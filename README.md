# RizzList

## Project Context

**RizzList** is a high-performance desktop application designed for task management. Its construction is based on the **C++23** standard, prioritizing memory safety and a strict modular architecture that decouples the graphics engine from the business logic.

The main technical objective of this architecture was to separate low-level responsibilities (rendering, window management) from specific business rules (users, tasks, database).

---

## System Architecture

The system is physically divided into two main modules managed by CMake: **Core** and **App**.

1.  **Core Module (Engine):** Acts as a static infrastructure library. It is data-agnostic and handles the application lifecycle and rendering orchestration.
2.  **App Module (Logic):** Contains the domain-specific "To-Do List" business logic and the user interface, residing in `App/Source`.

---

## Technical Features

* **Robust Persistence:** Uses SQLiteCpp for transactional data persistence.
* **List Management with "Soft Delete":** Implements a logical deletion pattern. Instead of physically removing records, the `isDeleted` column is marked, allowing for referential integrity maintenance and data restoration.
* **Security:** Implements a manual hashing system (djb2 algorithm) in the `AuthManager` to ensure passwords are never stored in plain text.
* **User Interface (UI):** Implements an Immediate Mode GUI using the raygui library over the raylib backend, defining logic and drawing in every frame.

---

## Tools and Technologies

* **Language:** C++23.
* **Build System:** CMake 3.28+.
* **Graphics Backend:** Raylib.
* **User Interface:** Raygui.
* **Database:** SQLiteCpp (SQLite).

---

## Build and Execution

The project uses CMake to manage external dependencies (Raylib, SQLiteCpp) automatically. To build from source:

```bash
# 1. Create build directory
mkdir build
cd build

# 2. Generate project files (Configuration)
# This will download Raylib and configure static linking
cmake ..

# 3. Compile the project (Release for maximum performance)
cmake --build . --config Release
````

-----

## Key Concepts Practiced

The project demonstrates the application of modern design patterns and advanced C++ techniques:

  * **Layered Architecture:** Use of the `Layer` class to define the base interface for all screens, allowing the engine to handle them polymorphically.
  * **Singleton Pattern:** Implemented in the `Application` class to guarantee a single instance responsible for managing the application lifecycle.
  * **RAII (Resource Acquisition Is Initialization):** Used in the `Window` class to ensure graphic resources are automatically released when the object is destroyed.
  * **DAO Pattern (Data Access Object):** The `TaskManager`, `ListManager`, and `AuthManager` classes encapsulate all SQL queries, isolating database complexity.
  * **Memory Safety:** Use of `std::unique_ptr` for memory management and `std::optional` for handling null return values, drastically reducing memory leaks.
  * **Deferred Action Queues:** Implementation of `m_PostFrameQueue` to execute state changes (such as layer transitions) safely outside the main iteration loop.
