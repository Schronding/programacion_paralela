# Answers to Code Comments & Doubts

**1. Is MPI part of the official C libraries?**
*Doubt:* "As MPI is very important I imagine it is on the official libraries of C"
*Answer:* MPI (Message Passing Interface) is actually not part of the standard C library. It is a separate standardization for parallel computing. You have to install an implementation like OpenMPI or MPICH and link against it (which is why you use `mpicc` instead of `gcc`).

**2. Asterisks and Ampersands in C**
*Doubt:* "I still don't understand what is the relationship between asterisks and ampersands in C."
*Answer:* In C, `&` means "get the memory address of this variable" (Address-of operator). The `*` in a declaration means "this is a pointer" (a variable that stores a memory address). When used on an existing pointer, `*` means "go to the address this pointer holds and get the value there" (Dereference operator). You use `&` to tell functions where your variables live so they can modify them. 

**3. argc and argv as arguments**
*Doubt:* "It is interesting that while the argc and argv values are present in every c program, I need to put them as arguments..."
*Answer:* In C, if you don't need command-line arguments, you can declare main as `int main(void)`. When you do need them, you must declare `argc` and `argv` so the operating system has clear variables to map the input arguments into. 

**4. Meaning of argc and argv**
*Doubt:* "I know that every main function requires these two arguments, in which if I recall well the first one was used to describe the values of the variables and the second is used as an array that has all the names of the variables"
*Answer:* Close! `argc` (argument count) is an integer representing the total number of command-line arguments passed to the program (including the program name itself). `argv` (argument vector) is an array of strings representing the actual arguments. `argv[0]` is the program name, `argv[1]` is the first parameter passed, etc.

**5. Execution scope of MPI processes**
*Doubt:* "For what I recall the total file is executed once for every 'computer' or 'processor' there is, so while size will remain stable in all the copies, the rank will change for each execution"
*Answer:* That is correct! The `mpirun` or `mpiexec` command launches multiple independent instances (processes) of your single program. Every process has the same `size` (the total number of processes in the communicator), but each process gets a unique `rank` (from 0 to size-1).

**6. Why MPI requires its own datatype (e.g. MPI_INT)**
*Doubt:* "I keep wondering about why the program requires to have its own datatype instead of simply checking which value is being sent and transforming it to the desired datatype..."
*Answer:* Unlike higher-level languages (like Python), standard C doesn't attach type metadata to variables in memory at runtime. In C, memory is just raw bytes. When you pass `&value` to `MPI_Bcast`, MPI only receives a raw memory address. It doesn't know what's stored there. You must explicitly pass `MPI_INT` so MPI knows how many bytes to read, and how to encode/decode those bytes if it's sending them to a different computer architecture with different endianness.

---

## User Research Summary: PRRTE, Slots, and Exascale

**7. What are slots and PRRTE?**
*Research Notes:*
- PRRTE stands for "PMIx Reference RunTime Environment", and PMIx is the "Process Management Interface for Exascale". 
- "Exascale" refers to exascale computing systems—supercomputers capable of at least one exaFLOPS (a quintillion calculations per second). OpenMPI/PMIx is designed to scale up to these massive computing architectures.
- PRRTE is the daemon responsible for discovering hardware, launching processes across the network, and wiring them so they can communicate via MPI.
- A "slot" is PRRTE's term for an allocatable unit to run a process. By default, PRRTE equates 1 slot = 1 physical core.

**8. Hyperthreading and Over-subscription**
*Research Notes:*
- While a processor might have 8 physical cores, hyperthreading (SMT) effectively creates two logical hardware threads per core. By default, PRRTE might only see the 8 physical cores. You can force it to recognize the hardware threads as slots using the `--use-hwthread-cpus` flag.
- **Oversubscription** occurs when you try to spawn more MPI processes than available slots. Since the extra processes aggressively compete for CPU time and context-switch frequently, efficiency drops. PRRTE blocks this by default to prevent performance degradation but can be bypassed with the `--oversubscribe` flag.

---

## Exercise 2 & Git Errors

**9. Correct names for MPI Rank and Size functions**
*Doubt:* "I don't recall well the names of the functions" (referring to fetching rank and size).
*Answer:* You were close with `MPI_Get_Size` and `MPI_Get_Rank`. The standard MPI functions you are looking for are actually `MPI_Comm_size(MPI_COMM_WORLD, &size);` and `MPI_Comm_rank(MPI_COMM_WORLD, &rank);`. Notice that the communicator (`MPI_COMM_WORLD`) goes first, followed by the memory address of the integer where the result should be stored.

**10. What is a Git object and what does the "empty object file / bad object HEAD" error mean?**
*Doubt:* The user experienced an error where `git status` crashed with `error: object file .git/objects/... is empty` and `fatal: bad object HEAD`, asking what a Git object is.
*Answer:* 
- **What is a Git object?** Git stores all of its data (commits, file contents/blobs, directory trees) in a hidden key-value database inside the `.git/objects` folder. Each specific piece of data is an "object" identified by a unique cryptographic hash.
- **Why the error?** This error usually happens if a computer crashes, loses power, unexpectedly shuts down, or runs out of disk space right in the middle of a Git operation. Git created the file for the object on your hard drive but was interrupted before it could write the data, leaving an empty, corrupted file. 
- **Bad object HEAD:** "HEAD" is a pointer to your current branch and its latest commit. Because the commit object file that HEAD is pointing to is empty/corrupt, Git cannot read the current state of your repository, causing a fatal error when you try to use `git status`.

**11. White spaces and newlines in C**
*Doubt:* "I remember that C is able to ignore white space... do I need to use a scape character or something like that to define a long function over several lines?"
*Answer:* You don't need any escape character! C completely ignores arbitrary whitespace, including tabs and new lines (Enters) that exist between syntax tokens. You did it perfectly: you can happily span a long function call over multiple lines to maintain readability.

**12. Why declare datatypes again?**
*Doubt:* "it confuses me even more that I need to declare the datatype of the sub array, isn't it obvious?..."
*Answer:* As we saw in exercise 1, standard C treats passing an array to a function merely as passing the starting memory address. `MPI_Scatter` only receives the raw pointer for `complete_array` and `sub_array`—it has absolutely no idea if those blocks of memory contain chars, floats, or integers. By specifying `MPI_INT`, you tell MPI exactly how many bytes equal one element so it can slice the data appropriately.

**13. Printing arrays in C**
*Doubt:* "I imagine there must be a better way to print the whole array in the printf function."
*Answer:* A bit surprisingly, no. Unlike modern high-level languages like Python where you can just `print(array)`, C's `printf` has no format specifiers for entire arrays. The standard, idiomatic way to print an array in C is strictly using a `for` loop that iterates over each element.

---

## User Research Summary: Processor Ordering & Scatter Mapping

**14. CPU Scheduling Order vs Data Determinism**
*Research Notes:*
- **The mixing outputs:** When you execute `mpirun` with 4 processes, they spin up concurrently as separate OS processes. Standard output (`stdout`) from each process competes to be printed to your terminal. Depending on tiny fluctuations in CPU scheduling, operating system loads, or network speeds, whichever rank finishes calculating and reaches the `printf` first prints first. Therefore, the order you see them normally feels "mixed" or random.
- **The deterministic data assignment:** Despite the "who prints first" race condition, `MPI_Scatter` behaves deterministically based strictly on the rank. Rank `0` gets the very first chunk of the array, Rank `1` the second chunk, and so forth. This sequential mapping guarantees data consistency every run, even if the print statements jumble in the console.

---

## Troubleshooting Git Corruption

**15. How to fix a Corrupt Git Object and Bad HEAD**
*Doubt:* "How can I solve the corrupt git repository problem? It's fine if the new head is my current repository version."
*Answer:* When Git stops with `fatal: bad object HEAD` and an empty object file (like `.../6e/dbe... is empty`), the latest commit was completely corrupted on disk. To fix it while keeping the current working directory intact:
1. Find the last known good commit before the crash by looking at the `.git/logs/refs/heads/main` (the reflog).
2. Manually overwrite the `main` branch pointer (`.git/refs/heads/main`) with the hash of the last good commit (`255cec8c0...`).
3. Delete the empty, corrupted object file (`rm .git/objects/6e/dbe...`).
Because Git doesn't touch your actual project files down in the working directory when you simply edit the `.git` metadata directly, all your current code changes are safely preserved as "uncommitted changes". You can now safely run `git add .` and `git commit` to create a fresh, uncorrupted commit of your current state.
