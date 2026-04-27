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

**16. Why does `git push` fail after restoring an old local commit?**
*Doubt:* "Can I force this commit to completely replace whatever is in the repo? I think I used fetch before but that didn't solved the problem... git push rejected (fetch first)."
*Answer:* 
- **The cause:** Because we artificially moved your local repository back in time to "delete" the corrupt commit, your local branch `main` is now technically missing the last commit that was already synced to the remote server on GitHub. Git assumes you are behind and rejects normal pushes because it doesn't want you to accidentally overwrite code on the server.
- **The solution:** Since we *do* want to overwrite the server repository intentionally with your newly uncorrupted local version, you have to bypass Git's safety net using a "force push". Running `git push --force origin main` (or `git push -f origin main`) commands the remote repository to completely mirror your local history, overwriting the conflicted timeline in GitHub entirely.

---

## Exercise 3: MPI_Gather & Output Control

**17. The arguments of MPI_Gather**
*Doubt:* "I put the order backwards... The correct order is: 1. Slice, 2. Quantity per node, 3. Datatype, 4. The whole array, 5. I don't know how to explain this, 6. Datatype, 7. Destination, 8. Bus of communication (Communicator). They are identical to MPI_Scatter."
*Answer:* You are correct! `MPI_Gather` has the exact same anatomy as `MPI_Scatter` but logically inversed in its internal mechanism. For argument #5 (which you didn't know how to explain): this is the **Receive Count per Node**. A common mistake is thinking it should be the total size of the final array (e.g., 16). It is not! It is the number of elements the root process receives *from each individual process* (which is 4 in this case). 
Also, for #8, the proper terminology in MPI is indeed **"Communicator"** mapping all the processes (rather than "bus", which is a hardware networking term, though conceptually similar).

**18. Conditional printing and avoiding output spam**
*Doubt:* "I think the smart way is simply to put an if statement so only rank 0 can print the complete array, as without the if, I will get a slot number of printed arrays in the console."
*Answer:* Spot on! Since the code is executed entirely by every process, anything outside an `if` block is run by everyone. If you hadn't wrapped the print statement in `if (rank == 0)`, all 4 processes would have tried to print the final array. Moreover, only rank 0 actually received the fully constructed array from `MPI_Gather` (since it was the target). The other ranks would have printed garbage memory since their `complete_array` was never updated by `MPI_Gather`! 

**19. Operator `*=` and its functionality**
*Doubt:* "It didn't work as I expected, probably because *= 10 is not doing what I expect..."
*Answer:* The compound assignment operator `a *= 10` is perfectly correct and behaves exactly like `a = a * 10`. It directly modifies the value inside the array at that specific memory location. Your logic in the loop was flawless.

**20. Formatting arrays outputs (The trailing comma)**
*Observation:* "The `, ` space at the end of the array is a bit awkward, but I think it is fine."
*Answer:* This is a classic C output formatting quirk. A common elegant trick to fix it is checking if it's the last iteration of the loop. For example: `printf("%d%s", complete_array[c], (c == ARRAY_SIZE - 1) ? "" : ", ");` will only print the comma if it's not the final element.

---

## Exercise 4: Random Numbers & MPI_Reduce

**21. Generating random numbers in C**
*Doubt:* "I recall there was a way to get random numbers in C fairly easy... a mix of division and a multiplication... edges of your random number generator."
*Answer:* To generate standard random numbers in C, you use the `<stdlib.h>` library for `rand()` and `srand()`, along with `<time.h>` to seed the generator (so it doesn't output the same sequence every execution). The math trick you are trying to remember to set the boundaries (edges) uses the **modulo operator (`%`)**. The formula to get a number between `MIN` and `MAX` is: 
`(rand() % (MAX - MIN + 1)) + MIN;`
For edges 1 and 100, it looks like: `(rand() % 100) + 1;`.

**22. Operator tokens for Maximum, Minimum, and Sum in MPI**
*Doubt:* "I think I recall the functions to get the maximum and minimum were MPI_MAX and MPI_MIN respectively."
*Answer:* Exactly! `MPI_MAX` and `MPI_MIN` are the correct predefined reduction operation handles in MPI. For summation (as requested in the exercise to add all values), you use `MPI_SUM`.

**23. Arguments of MPI_Reduce**
*Doubt:* "I don't recall the arguments of MPI_Reduce either... I imagine I need a buffer to store the data, a datatype..."
*Answer:* You imagine correctly! `MPI_Reduce` consolidates scattered data from all processes into a single variable at the root process. The signature requires 7 arguments:
1. `sendbuf`: The local variable holding this process's contribution (e.g., `&my_random_number`).
2. `recvbuf`: The root variable where the final answer is stored (e.g., `&max_result`). Only the root's buffer actually gets the answer!
3. `count`: Number of elements to reduce (just `1` in this case).
4. `datatype`: Data type of the variable (e.g., `MPI_INT`).
5. `op`: The mathematical operation to run (e.g., `MPI_MAX` or `MPI_SUM`).
6. `root`: ID of the process that will receive the final answer (usually `0`).
7. `comm`: The communicator network (`MPI_COMM_WORLD`).

**24. Seeding random numbers properly in MPI (`time.h`)**
*Doubt:* "I know I need time in order to get a unique seed... but I don't know how to do so."
*Answer:* In standard C, you seed the generator using `srand(time(NULL));` at the beginning of `main()`. However, in MPI, all processes are launched simultaneously. If they all call `time(NULL)`, they will all receive the exact same second and initialize with the identical seed, meaning they will all generate the exact same "random" numbers. To fix this, always add the rank to the seed in MPI: `srand(time(NULL) + rank);`.

**25. Why is the random number going out of bounds?**
*Observation:* "...the bounds of my array aren't limiting the range correctly..."
*Answer:* The formula `(rand() % (random_ceiling - random_floor + 1))` generates a chunk of numbers scaled from 0 up to 99 in your case. You forgot to add the minimum (`+ random_floor`) outside the modulo at the very end to shift the range up to 1-100.

**26. Why am I getting identical minimums and maximums (e.g., 83)?**
*Observation:* "Of those, the minimum is 83 and the maximum is 83"
*Answer:* Since you didn't execute `srand()`, C defaulted to a seed of `1` for *every* process. Therefore, every single one of your 16 processes calculated the exact same random number (83). The maximum of {83, 83, 83...} is 83, and the minimum is 83!

**27. Why am I getting garbage values in the printed array?**
*Observation:* "I am getting garbage values... [83, 0, 1303831994, 29586...]"
*Answer:* You declared `int array[THREADS]` locally in every process, but each process only populates *one* slot (`array[rank] = ...`). The remainder of the array is never initialized in memory. `MPI_Reduce` consolidates a single mathematical result (`min`/`max`), it *does not* consolidate arrays like `MPI_Gather` does! Since rank 0 tries to print the whole array, it prints its own element `array[0]` (the 83) and then raw uninitialized memory for the rest.

**28. Implicit Synchronization in MPI**
*Doubt:* "I think that each of the copies of this program are running probably concurrently or there are implicit synchronization barriers... otherwise it might print garbage."
*Answer:* Your intuition is exactly right. Collective operations like `MPI_Reduce`, `MPI_Gather`, and `MPI_Bcast` have an implicit synchronization barrier. Rank 0 literally cannot finish the `MPI_Reduce` function call until it has received the payload from all other participating ranks. They wait for each other.

**29. The "void value not ignored" error in `srand()`**
*Doubt:* "...error: void value not ignored as it ought to be... I don't see how telling a function time that there is no time (by putting NULL) is going to help me get an unique seed..."
*Answer:* The error triggers because `srand()` returns `void` (it doesn't return anything). You tried to do `srand(...) + rank`, which tells C to mathematically add `rank` to "nothing". You must do the addition *inside* the parentheses: `srand(time(NULL) + rank);`. Also, passing `NULL` to `time()` does not mean "no time"; it means "I don't want you to store the time in a pointer, just return it directly".

**30. Why is `rank` returning garbage values for the seed?**
*Answer:* If you look closely at your code, you execute `srand(time(NULL) + rank);` on line 12, but you only assign a value to `rank` with `MPI_Comm_rank` on line 14! Because you used `rank` before initializing it, it passed uninitialized memory garbage as your seed, completely breaking your randomization sequence.

**31. `max` and `sum` getting garbage values in `MPI_Reduce`**
*Doubt:* "...my `max` and `sum` the ones who are getting garbage values..."
*Answer:* For `MPI_Reduce`, each processor is supposed to send its individual slice of data. You wrote `MPI_Reduce(&array[rank], ...)`. However, only rank 0 contains the populated `array` (because of `MPI_Gather` just before it). For all the other 15 ranks, `array[rank]` is completely empty/uninitialized memory! The correct variable to send to the reduction process is simply `&random_value`, which holds the local process's data.

**32. The bizarre "rank 4" printing error**
*Observation:* "...I am still getting that strange rank 4 error... I am rank 4 and the array of random values is..."
*Answer:* The `if (rank == 0)` block perfectly isolated the code, so it was definitely rank 0 doing the printing. However, your print statement is `printf("I am rank %d...", "[");`. You completely forgot to pass the variable `rank` as an argument to `printf`! Because C doesn't strictly verify variable counts in `printf`, it just grabbed whatever rogue number was sitting in the next memory register or stack, which happened to evaluate to a `4`. You need to use `printf("I am rank %d and the array of random values is [", rank);`.

**33. What does PRRTE Slots and `--use-hwthread-cpus` mean here? (Your research)**
*Observation:* "There are not enough slots available in the system to satisfy the 16 slots that were requested... I use [use-hwthread-cpus] because when I simply use -np 16 I am not allowed..."
*Answer:* Standard OpenMPI daemon (PRRTE) sets 1 slot = 1 physical CPU core by default to prevent performance loss from process competition (oversubscription). Because your CPU has 8 physical cores (but 16 threads via hyper-threading), requesting 16 nodes directly makes PRRTE crash defensively. By using `--use-hwthread-cpus`, you are explicitly altering the daemon's rules, instructing it to treat the 16 virtual threads as 16 actual compute slots, perfectly solving your problem.

---

## Exercise 4: Final Realizations

**34. Sending local data vs arrays in MPI Collectives**
*Observation:* "Only rank 0 has a complete array, the others do not... I just need to send whatever value the slot calculated!"
*Answer:* Perfect realization! In MPI, variables are strictly local to each process's memory space. Only rank 0's `array` was fully populated (by `MPI_Gather`). Collective reduction operations (`MPI_Reduce`) are explicitly designed to take a single local scalar/buffer from *each* rank (`&random_value`) and collapse them all into a single mathematical result at the root.

**35. Missing variables in `printf` format strings**
*Observation:* "My error getting the value 4 was that I never passed the rank variable to fill the placeholder!"
*Answer:* Exactly! In C, `printf` blindly relies on the variables you explicitly pass to match its `%d` or `%s` format specifiers. If you omit the variable in the function call, it will reach out and read adjacent memory (registers or the stack), leading to unpredictable "phantom" numerical outputs like your `4`.

---

## Exercise 5: MPI_Allreduce & C Formatting

**36. Using `man` for MPI functions in the Terminal**
*Research Notes:* Yes! If the OpenMPI documentation is installed correctly on your system, you can pull up the manual for any signature directly via the terminal by typing `man MPI_Allreduce`. It specifies the arguments, valid op-flags, and potential errors.

**37. The simplified arguments of `MPI_Allreduce` vs `MPI_Gather`**
*Doubt:* "What I don't understand is why I don't need to specify the datatypes and the count of sending/receiving messages such as in other MPI functions."
*Answer:* Functions like `MPI_Gather` and `MPI_Scatter` concatenate (join) arrays, meaning the "received" data size is vastly different from the "sent" data size. `MPI_Reduce` and `MPI_Allreduce` perform mathematical combinations element-by-element (e.g., overlapping them to sum them up). If you send 1 element, the final reduced buffer is still exactly 1 element. Because the send and receive lengths and datatypes are forced to be identical, MPI developers simplified the signature: you only declare `count` (1) and `datatype` (`MPI_INT`) once.
Also, your intuition was correct: `MPI_Allreduce` is identical to `MPI_Reduce`, it just automatically broadcasts the final answer back to *every* node at the end. That is why it does not require the `root` argument.

**38. Escaping the percentage sign `%` in `printf`**
*Doubt:* "It seems that I will get an error for the percentage sign. I tried to escape it but it doesn't seem it is enough (\%)."
*Answer:* In C strings, the backslash `\` escapes text-parser characters like `\n` (newline) or `\t` (tab). However, the `%` symbol is a special token consumed by the `printf` formatter itself. To "escape" it so `printf` physically prints it, you must double it: `%%`.

**39. Logical error in percentage calculation (Integer Division)**
*Doubt:* "I am getting a logical error but I don't know if it is because I am writing incorrecly the percentage formula..."
*Answer:* Your formula `(work_load * 100 / total_work)` suffers from pure "integer division". Because both `work_load` and `total_work` are integers, the C compiler computes them purely in whole numbers and destroys the invisible decimals *before* assigning it to your `float percentage`. To fix this, you just need to turn one of the factors into a floating-point number. Changing `100` to `100.0` will force the compiler to execute floating-point division instead: `(work_load * 100.0 / total_work)`.

**40. Formatting floats to 2 decimal places in `printf`**
*Doubt:* "...Also I don't recall how do I write so the printed value is truncated to just 2 decimals."
*Answer:* You wrote `%f:2f`, which `printf` evaluates literally as "Format a float here (`%f`), and then simply write the string ':2f'". To limit decimals, place `.2` between the `%` and `f`, which acts as a precision specifier: `%.2f`.

---

## Exercise 5: Final Realizations

**41. Solving Integer Truncation naturally**
*Observation:* "Indeed it seems that what was missing the values was that I was eating some decimals by not putting any value of the percentage calculation as float."
*Answer:* Exactly! In C, operations involving only integers automatically discard the fractional remainder (truncation). As soon as you introduced `100.0` (a floating-point constant), you implicitly cast the entire arithmetic expression into an operation governed by standard floating-point rules, preserving your critical decimal accuracy.

**42. Correct Percentage Logic**
*Observation:* "I had the percentage logic backwards before: it is (particular / total) * 100"
*Answer:* You hit the nail on the head! The logic is the ratio (`part / whole`) scaled to a fraction out of 100. Due to C's operator precedence (Left-to-Right for multiplication and division), writing `(work_load * 100.0 / total_work)` mathematically evaluates the exact same way as `(work_load / total_work) * 100.0`, but is actually safer in C because it avoids dividing a tiny integer by a large integer first (which might prematurely truncate to 0 before the multiplication if you forgot the `.0`). So your code formulation is the most robust way to calculate it!
