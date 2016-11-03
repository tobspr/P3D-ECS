"""

Generates various testcase-variations for the collecors and components testcase (testcase_collectors.cpp)

"""

operations = {
    "CE": "ConstructEntity",
    "UM": "UpdateManager",
    "ATC": "AddTransformComponent",
    "DE": "DeleteEntity",
    "RTC":" RemoveTransformComponent",
    "EXP_0": "ProcessAndExpect0",
    "EXP_1": "ProcessAndExpect1"
}

def num_params(n):
    return range(2**n)

def param(i, n):
    return (i // (2**n)) % 2 == 0

out = ""

for i in num_params(3):

    update_after_creation = param(i, 0)
    remove_transform_before_deletion = param(i, 1)
    update_after_transform_removal = param(i, 2)

    sequence += ["CE"]

    if update_after_creation:
        sequence += ["UM"]

    sequence += ["EXP_0"]

    sequence += ["ATC"]

    sequence += ["EXP_0"]

    sequence += ["UM"]

    sequence += ["EXP_1"]

    if remove_transform_before_deletion:
        sequence += ["RTC"]

    sequence += ["EXP_1"]
    
    sequence += ["UM"]

    sequence += 

    sequence += ["DE"]

    sequence += ["EXP_1"]
    
    sequece += ["UM"]

    sequence += ["EXP_0"]
    

    if sequence[-1] != "UM":
        sequence += ["UM"]
    
    parsed_seq = ["Operation::" + operations[i] for i in sequence]
    
    out += "{"
    out += ", ".join(parsed_seq)
    out += "}, // Testcase " + str(i + 1) + "\n"

with open("testcase_collectors_output.txt", "w") as handle:
    handle.write(out)
