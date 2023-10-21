# petitdb

---

petitdb is a simple command-line database management system implemented in C that I wrote for one of my classes (I'm planning on working on it in the future so I gave it a name). It allows you to perform basic operations such as inserting records, selecting records based on certain criteria, and sorting records.

**To-Do**:

* Refactor the code into multiple files for better organization.
* Explore more advanced data structures like B+ trees for representing the database.
* Improve the search functionality with more advanced queries.
* Add additional commands for database manipulation.
* Enhance the command syntax for better user experience.

**Features**:

* Insert records: insert new records into the database by specifying the ID, name, and score of the record.
* Select records: select records based on different attributes such as ID, name, score, or select all records.
* Sort records: sort the records in the database based on a specified attribute.

**Usage**:

* Clone the repository: `git clone https://github.com/sardonyx001/petitdb.git`
* Navigate to the project directory: `cd petitdb`
* Compile the code: `gcc [main.c](main.c) -o petitdb`
* Run the program: `./petitdb`

**Commands**:

* `exit`: Exits the program.
* `select [attribute] [value]`: Selects records based on the specified attribute and value. Available attributes are id, name, score, and * (select all records).
* `sort [attribute]`: Sorts the records in the database based on the specified attribute. Available attributes are id, name, and score.
* `insert [id] [name] [score]`: Inserts a new record into the database with the specified ID, name, and score.

**Examples**:

* `select id 5`: Selects records with ID equal to 5.
* `select name John`: Selects records with name equal to "John".
* `select score 90`: Selects records with score equal to 90.
* `select *`: Selects all records.
* `sort id`: Sorts the records in ascending order based on ID.
* `sort name`: Sorts the records in alphabetical order based on name.
* `sort score`: Sorts the records in ascending order based on score.
* `insert 10 Alice 80`: Inserts a new record with ID 10, name "Alice", and score 80 into the database.

**Challenges**:

* strtok tokenizes based on each "\n," so you need to consider this.
  * Solution: Add "\n" to the sep parameter of `strtok`: `strtok(cmd, " \n")`
* To continue processing, parsedTokens needs to be initialized each time.
  * Solution: Set it to `NULL` in `parseCmd()`.
  * `if (parsed != NULL) parsed = NULL;`
* You must include break at the end of each case statement in the switch statement.
* Be careful with the array range when using q_sort.
  * (The last index is not `db->size` but `db->size-1`.)
* In the do_select function, I tried to make use of a function lookup table (LUT). However, the select function is searching for the actual value of the specified attribute, while the cmpFuncs LUT is used to compare records with each other. Thus the work needed to identify the record where the desired value is located at becomes a straightforward search, rendering the use of the LUT unnecessary.

**References**:

* [Stack Overflow - How can I implement a function lookup table in C?](https://stackoverflow.com/questions/66108949/how-can-i-implement-a-function-lookup-table-in-c
)
* [Stack Overflow - Tokenizing strings in C](https://stackoverflow.com/questions/266357/tokenizing-strings-in-c)
* [Stack Overflow - Copying arrays of structs in C](https://stackoverflow.com/questions/1693853/copying-arrays-of-structs-in-c)
* [Stack Overflow - Removing trailing newline character from fgets input](https://stackoverflow.com/questions/2693776/removing-trailing-newline-character-from-fgets-input)
* [Stack Overflow - Sorting members of structure array](https://stackoverflow.com/questions/13372688/sorting-members-of-structure-array)
* [Stack Overflow - Why am I getting "arithmetic of pointer type is required"?](https://stackoverflow.com/questions/9594427/why-am-i-getting-arithmetic-of-pointer-type-is-required)
