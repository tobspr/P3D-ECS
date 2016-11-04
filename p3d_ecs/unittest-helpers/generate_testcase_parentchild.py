
operations = {
    "CR": "ConstructRoot",
    "CC": "ConstructChild",
    "RC": "ReparentChild",
    "U": "Update",
    "DR": "DeleteRoot",
    "DC": "DeleteChild",
}

def num_params(n):
    return range(2**n)

def param(i, n):
    return (i // (2**n)) % 2 == 0

out = ""



for i in num_params(6):

    update_between_creation = param(i, 0)    
    update_after_creation = param(i, 1)
    create_root_first = param(i, 2)
    update_between_deletion = param(i, 3)
    delete_root_first = param(i, 4)
    update_after_reparent = param(i, 5)

    sequence = []
    if create_root_first:
        sequence += ["CR"]
    else:
        sequence += ["CC"]
    
    if update_between_creation:
        sequence += ["U"]
    
    if create_root_first:
        sequence += ["CC"]
    else:
        sequence += ["CR"]
    
    if update_after_creation:
        sequence += ["U"]

    sequence += ["RC"]

    if update_after_reparent:
        sequence += ["U"]

    if delete_root_first:
        sequence += ["DR"]
    else:
        sequence += ["DC"]
    
    if update_between_deletion:
        sequence += ["U"]

    if delete_root_first:
        #sequence += ["DC"]
        # child should be deleted now
        pass
    else:
        sequence += ["DR"]

    if sequence[-1] != "U":
        sequence += ["U"]
    
    parsed_seq = ["Operation::" + operations[i] for i in sequence]
    
    out += "{"
    out += ", ".join(parsed_seq)
    out += "}, // Testcase " + str(i + 1) + "\n"

with open("output.txt", "w") as handle:
    handle.write(out) 
