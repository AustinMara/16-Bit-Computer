import json
import os

autograding = {
  "tests": [
    {
      "name": "Test 1. Test bits",
      "setup": "",
      "run": "make test-bits",
      "input": "",
      "output": "",
      "comparison": "included",
      "timeout": 5,
      "points": 5
    },
    {
      "name": "Test 2. Test instruction",
      "setup": "",
      "run": "make test-instruction",
      "input": "",
      "output": "",
      "comparison": "included",
      "timeout": 5,
      "points": 2
    },
    {
      "name": "Test 3. Test control for ADD instruction",
      "setup": "",
      "run": "make test-control-add",
      "input": "",
      "output": "",
      "comparison": "included",
      "timeout": 5,
      "points": 10
    },
    {
      "name": "Test 4. Test control for AND instruction",
      "setup": "",
      "run": "make test-control-and",
      "input": "",
      "output": "",
      "comparison": "included",
      "timeout": 5,
      "points": 10
    },
    {
      "name": "Test 5. Test control for BR instruction",
      "setup": "",
      "run": "make test-control-br",
      "input": "",
      "output": "",
      "comparison": "included",
      "timeout": 5,
      "points": 10
    },
    {
      "name": "Test 6. Test control for JMP instruction",
      "setup": "",
      "run": "make test-control-jmp",
      "input": "",
      "output": "",
      "comparison": "included",
      "timeout": 5,
      "points": 10
    },
    {
      "name": "Test 7. Test control for JSR instruction",
      "setup": "",
      "run": "make test-control-jsr",
      "input": "",
      "output": "",
      "comparison": "included",
      "timeout": 5,
      "points": 10
    },
    {
      "name": "Test 8. Test control for LD instruction",
      "setup": "",
      "run": "make test-control-ld",
      "input": "",
      "output": "",
      "comparison": "included",
      "timeout": 5,
      "points": 10
    },
    {
      "name": "Test 9. Test control for LDI instruction",
      "setup": "",
      "run": "make test-control-ldi",
      "input": "",
      "output": "",
      "comparison": "included",
      "timeout": 5,
      "points": 10
    },
    {
      "name": "Test 10. Test control for LDR instruction",
      "setup": "",
      "run": "make test-control-ldr",
      "input": "",
      "output": "",
      "comparison": "included",
      "timeout": 5,
      "points": 10
    },
    {
      "name": "Test 11. Test control for ST instruction",
      "setup": "",
      "run": "make test-control-st",
      "input": "",
      "output": "",
      "comparison": "included",
      "timeout": 5,
      "points": 10
    },
    {
      "name": "Test 12. Test control for STI instruction",
      "setup": "",
      "run": "make test-control-sti",
      "input": "",
      "output": "",
      "comparison": "included",
      "timeout": 5,
      "points": 10
    },
    {
      "name": "Test 13. Test control for STR instruction",
      "setup": "",
      "run": "make test-control-str",
      "input": "",
      "output": "",
      "comparison": "included",
      "timeout": 5,
      "points": 10
    },
    {
      "name": "Test 14. Run emulator on test.obj in samples",
      "setup": "make",
      "run": "./x16 test/samples/test.obj",
      "input": "1 + 1",
      "output": "**********HALT",
      "comparison": "included",
      "timeout": 5,
      "points": 10
    },
    {
      "name": "Test 15. Test assembler with just one line of code",
      "setup": "make xas",
      "run": "make ARGS=\"Xas.simple\" test",
      "input": "",
      "output": "",
      "comparison": "included",
      "timeout": 5,
      "points": 10
    },
    {
      "name": "Test 16. Test assembler with comments",
      "setup": "make xas",
      "run": "make ARGS=\"Xas.comment\" test",
      "input": "",
      "output": "",
      "comparison": "included",
      "timeout": 5,
      "points": 10
    },
    {
      "name": "Test 17. Test assembler with unused prior label",
      "setup": "make xas",
      "run": "make ARGS=\"Xas.prior\" test",
      "input": "",
      "output": "",
      "comparison": "included",
      "timeout": 5,
      "points": 10
    },
    {
      "name": "Test 18. Test assembler with multiple instructions",
      "setup": "make xas",
      "run": "make ARGS=\"Xas.multi\" test",
      "input": "",
      "output": "",
      "comparison": "included",
      "timeout": 5,
      "points": 10
    },
    {
      "name": "Test 19. Test assembler with forward labels",
      "setup": "make xas",
      "run": "make ARGS=\"Xas.forward\" test",
      "input": "",
      "output": "",
      "comparison": "included",
      "timeout": 5,
      "points": 10
    },
    {
      "name": "Test 20. Test assembler with loop program",
      "setup": "make xas",
      "run": "make ARGS=\"Xas.loop\" test",
      "input": "",
      "output": "",
      "comparison": "included",
      "timeout": 5,
      "points": 10
    },
    {
      "name": "Test 21. Test assembler with all instructions",
      "setup": "make xas",
      "run": "make ARGS=\"Xas.all\" test",
      "input": "",
      "output": "",
      "comparison": "included",
      "timeout": 5,
      "points": 10
    },
    {
      "name": "Test 22. Test assembler with an error with nonexistent label",
      "setup": "make xas",
      "run": "make ARGS=\"Xas.error.nolabel\" test",
      "input": "",
      "output": "",
      "comparison": "included",
      "timeout": 5,
      "points": 10
    },
    {
      "name": "Test 23. Test assembler for an error with missing % before reg",
      "setup": "make xas",
      "run": "make ARGS=\"Xas.error.reg\" test",
      "input": "",
      "output": "",
      "comparison": "included",
      "timeout": 5,
      "points": 10
    },
    {
      "name": "Test 24. Test assembler for an error with missing $ before value",
      "setup": "make xas",
      "run": "make ARGS=\"Xas.error.imm\" test",
      "input": "",
      "output": "",
      "comparison": "included",
      "timeout": 5,
      "points": 10
    },
    {
      "name": "Test 25. Test style",
      "setup": "sudo pip3 install cpplint",
      "run": "cpplint *.c *.h",
      "input": "",
      "output": "",
      "comparison": "included",
      "timeout": 10,
      "points": 10
    },
    {
      "name": "Test 26. Test X16 Memory",
      "setup": "make x16",
      "run": "valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=1 ./x16 test/samples/test.obj",
      "input": "",
      "output": "",
      "comparison": "included",
      "timeout": 10,
      "points": 5
    },
    {
      "name": "Test 27. Test Xas Memory",
      "setup": "make x16",
      "run": "valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=1 ./xas test/samples/test.s",
      "input": "",
      "output": "",
      "comparison": "included",
      "timeout": 10,
      "points": 8
    }
  ]
}


def runtest(test):
    print(test["name"] + ", " + str(test["points"]) + " points")
    if test["setup"].startswith("make"):
        os.system(test["setup"])
    if os.system(test["run"] + " > out") != 0:
        return 0
    if test["output"] != "":
        grepcmd = "grep \"" + test["output"] + "\" out"
        if os.system(grepcmd) != 0:
            return 0
    return test["points"]

def main():
    score = 0
    for test in autograding["tests"]:
        score += runtest(test)
    print("Final score=" + str(score))

if __name__ == "__main__":
    main()
