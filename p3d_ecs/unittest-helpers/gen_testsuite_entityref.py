"""

Generates the various entity-ref testcases

"""

def num_params(n):
    return range(2**n)

def param(i, n):
    return (i // (2**n)) % 2 == 0

out = ""


ttypes = ["empty", "nullptr", "empty_id", "id", "reg_id", "entity", "reg_entity"]

k = 0
for input_type in ttypes:

    for reset_param in ttypes:

        for i in num_params(2):

            use_get_ptr_instead_of_fill_ptr = param(i, 0)
            fill_or_get_after_reset = param(i, 1)


            # First, prepare everything
            sequence = []

            if input_type in ["id", "reg_id", "entity", "reg_entity"]:
                sequence += ["CreateEntity1"]

            if input_type in ["reg_id", "reg_entity"]:
                # register entity first
                sequence += ["MgrProcessChanges"]

            if input_type == "empty":
                sequence += ["CreateRefFromEmpty"]
            elif input_type == "nullptr":
                sequence += ["CreateRefFromNullptr"]
            elif input_type == "empty_id":
                sequence += ["CreateRefFromEmptyId"]
            elif input_type == "id":
                sequence += ["CreateRefFromId"]
            elif input_type == "reg_id":
                sequence += ["CreateRefFromId"]
            elif input_type == "entity":
                sequence += ["CreateRefFromEntityPtr"]
            elif input_type == "reg_entity":
                sequence += ["CreateRefFromEntityPtr"]
            else:
                print("ERROR UNKOWN INPUT TYPE", input_type)

            reset_param_is_registered = False

            # Prepare reset parameter
            if reset_param in ["id", "reg_id", "entity", "reg_entity"]:
                sequence += ["CreateEntity2"]

            if reset_param in ["reg_id", "reg_entity"]:
                sequence += ["MgrProcessChanges"]

            if reset_param == "empty":
                sequence += ["ResetRefFromEmpty"]
            elif reset_param == "nullptr":
                sequence += ["ResetRefFromNullptr"]
            elif reset_param == "empty_id":
                sequence += ["ResetRefFromEmptyId"]
            elif reset_param == "id":
                sequence += ["ResetRefFromId"]
            elif reset_param == "reg_id":
                sequence += ["ResetRefFromId"]
                reset_param_is_registered = True
            elif reset_param == "entity":
                sequence += ["ResetRefFromEntityPtr"]
            elif reset_param == "reg_entity":
                sequence += ["ResetRefFromEntityPtr"]
                reset_param_is_registered = True
            else:
                print("ERROR UNKOWN RESET TYPE", reset_param)


            has_full_pointer = reset_param in ["entity", "reg_entity"]

            if fill_or_get_after_reset:
                if reset_param in ["id", "reg_id"]:
                    if use_get_ptr_instead_of_fill_ptr:
                        sequence += ["RefGetPtr"]
                    else:
                        sequence += ["RefFillPtr"]
                    has_full_pointer = True
                    sequence += ["ValidateRefToEntity2"]

            if has_full_pointer:
                sequence += ["ValidateRefToEntity2"]
            elif reset_param in ["id", "reg_id"]:
                sequence += ["ValidateRefToEntity2IdOnly"]
            else:
                sequence += ["ValidateRefEmpty"]

            # First, delete entity 1
            if "CreateEntity1" in sequence:
                sequence += ["RemoveEntity1"]

            # Now, delete the second entity
            if "CreateEntity2" in sequence:
                sequence += ["RemoveEntity2"]

            if reset_param == "id":
                if has_full_pointer:
                    if not reset_param_is_registered:
                        sequence += ["ValidateRefEmpty"]
                    else:
                        sequence += ["ValidateRefToEntity2"]
                        sequence += ["MgrProcessChanges"]
                        sequence += ["ValidateRefEmpty"]
                else:
                    # Entity ptr already deleted, can not compare
                    # sequence += ["ValidateRefToEntity2IdOnly"]
                    pass

            elif reset_param == "reg_id":

                if has_full_pointer:
                    if not reset_param_is_registered:
                        sequence += ["ValidateRefEmpty"]
                    else:
                        sequence += ["ValidateRefToEntity2"]
                        sequence += ["MgrProcessChanges"]
                        sequence += ["ValidateRefEmpty"]
                else:
                    # Entity ptr already deleted, can not compare
                    # sequence += ["ValidateRefToEntity2IdOnly"]
                    pass

            elif reset_param == "entity":
                # unregistered entities are deleted instantly
                sequence += ["ValidateRefEmpty"]

            elif reset_param == "reg_entity":
                # registered entities take an update to get deleted
                sequence += ["ValidateRefToEntity2"]

                sequence += ["MgrProcessChanges"]
                sequence += ["ValidateRefEmpty"]

            else:
                # should be empty already
                sequence += ["ValidateRefEmpty"]


            k += 1

            parsed_seq = ["Operation::" + _k for _k in sequence]

            out += "// Testcase " + str(k) + ", i=" + input_type + ", r=" + reset_param + ", i=" + str(i) + "\n"
            out += "{"
            out += ", ".join(parsed_seq)
            out += "},\n"

out = out.rstrip(",\n ")

with open("output.txt", "w") as handle:
    handle.write(out)
