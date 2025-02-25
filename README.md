**MOVIE RECOMMENDATION SYSTEM**  
=============================================  
### **-Project Description-**  
The project processes user-movie interactions given as input using a **bipartite graph** structure and provides movie recommendations through **five different algorithms**.  

### **-Project Requirements-**  
- C++ compiler (**C++17 or later**)  
- Standard C++ libraries  

### **FILE DESCRIPTIONS**  
=============================================  
**-u.data-**  
This is the dataset used in the project. It contains **100,000 user-movie interactions**.  

**-u.info-**  
This file stores the **number of users and movies** in the dataset.  

**-u.item-**  
This file contains **detailed descriptions** of each movie in the dataset.  

**-main.cpp-**  
This is the main code file where the **graph structure is created, time measurements are performed, and recommendation algorithms are executed**.  

**-graph.cpp—movie.cpp—graph.h—movie.h-**  
These files contain **function and class definitions**.  

### **COMPILING THE PROJECT**  
=============================================  
Before compiling the project, make sure that **all the files mentioned above are in the same directory**.  

#### **-Compilation Command-**  
```bash
g++ -std=c++17 main.cpp graph.cpp movie.cpp -o rec_system
```

#### **-Execution Command-**  
```bash
./rec_system
```

#### **-Parameters-**  
After running the program, enter the **user ID** in the terminal to receive recommendations.  
Two macros defined in the **graph.cpp** file can be modified to:  
- Set the **minimum number of ratings** a recommended movie must have.  
- Change the **number of recommended movies**.  

#### **-Expected Output Files-**  
**-graph.txt-**: This file contains the **output of the graph structure**.  
**-recommendations.txt-**: This file contains the **movie recommendations** generated for the entered user.  
