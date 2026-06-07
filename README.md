# Parallelizing a Columnar Database Query Engine: An OpenMP Performance Analysis on Data Skew

## Overview

This project investigates how **OpenMP** can be used to accelerate analytical database queries on modern multi-core CPUs. A simplified **column-oriented query engine** was implemented in **C**, and its filtering operation was parallelized using OpenMP.

The project focuses on understanding:

* How OpenMP improves query execution performance
* The effects of **thread scalability**
* The impact of different **scheduling strategies**
* How **data skew** influences load balancing and overall efficiency

---

## Problem Statement

The engine evaluates the following query over a synthetic dataset containing **20 million user records**:

```sql
SELECT COUNT(*)
FROM Users
WHERE age >= 30
  AND balance > 50000;
```

The dataset is stored using a **columnar layout**, where each attribute is maintained in a separate contiguous array:

* `age_col[]`
* `balance_col[]`

This layout minimizes unnecessary memory accesses and is well suited for parallel processing.

---

## Why This Problem is Parallelizable

Each row can be evaluated independently:

* No shared writes between iterations
* No loop-carried dependencies
* No ordering constraints

This makes the workload an example of an **embarrassingly parallel** problem.

---

## Simulating Data Skew

To study scheduling behavior under imbalanced workloads, a deliberate skew was introduced:

* **Rows 0–9,999,999:** fail the predicate immediately (light workload)
* **Rows 10,000,000–19,999,999:** satisfy the predicate and execute an additional computationally intensive block involving repeated `sin()` and `cos()` calculations.

This setup allows comparison of different OpenMP scheduling strategies under realistic load imbalance conditions.

---

## OpenMP Implementation

The sequential filtering loop was parallelized using:

```c
#pragma omp parallel for \
    private(i) \
    shared(age_col, balance_col) \
    reduction(+:match_count) \
    schedule(...)
```

### OpenMP Concepts Used

* Parallel regions
* Fork–Join execution model
* Work-sharing (`parallel for`)
* Shared and private variables
* Reduction operations
* Scheduling policies:

  * Static
  * Dynamic
  * Guided

### Synchronization Strategy

Several synchronization approaches were evaluated:

| Method      | Reason Not Selected               |
| ----------- | --------------------------------- |
| `critical`  | Excessive serialization overhead  |
| `atomic`    | High contention in tight loops    |
| `barrier`   | Unnecessary synchronization       |
| `reduction` | Efficient and scalable solution ✅ |

---

## Experimental Setup

| Component        | Specification              |
| ---------------- | -------------------------- |
| CPU              | Intel Core i5              |
| Physical Cores   | 12                         |
| RAM              | 8 GB Unified Memory        |
| Operating System | macOS Sequoia 15.4         |
| Compiler         | Clang/LLVM 22.1.6          |
| OpenMP Support   | Homebrew LLVM (`-fopenmp`) |

### Compilation

```bash
clang -O3 -fopenmp query_engine.c -o query_engine -lm
```

### Execution

```bash
./query_engine
```

All experiments were repeated **5 times**, and average values were reported.

---

## Performance Results

### Thread Scalability (20 Million Rows)

| Threads | Time (s) | Speedup | Efficiency |
| ------- | -------- | ------- | ---------- |
| 1       | 0.0203   | 1.00×   | 100%       |
| 2       | 0.0116   | 1.75×   | 88%        |
| 4       | 0.0088   | 2.31×   | 58%        |
| 8       | 0.0080   | 2.53×   | 32%        |
| 16      | 0.0074   | 2.72×   | 17%        |

### Scheduling Comparison (8 Threads)

| Schedule | Execution Time (s) |
| -------- | ------------------ |
| Static   | 0.0077             |
| Dynamic  | **0.0054**         |
| Guided   | 0.0067             |

---

## Key Findings

### Dynamic Scheduling Performs Best

Dynamic scheduling outperformed static scheduling by approximately **30%** due to its ability to balance uneven workloads caused by data skew.

### More Threads Do Not Always Mean Better Performance

Although execution time continued to decrease beyond 8 threads, efficiency dropped significantly.

The workload eventually became **memory-bandwidth bound**, limiting scalability.

### Optimal Thread Count

For this workload and hardware configuration, **4–8 threads** provided the best balance between performance improvement and resource utilization.

---

## Correctness Verification

The sequential and parallel implementations were validated by comparing query results.

Both versions consistently returned:

```text
10,000,000 matching records
```

across all thread configurations.

---

## Relation to Real Database Systems

Many modern analytical database systems employ similar concepts:

* **Vectorized execution**
* **Massively Parallel Processing (MPP)**
* **Dynamic load balancing / work stealing**
* **Column-oriented storage layouts**

Examples include:

* Snowflake
* ClickHouse
* Amazon Redshift

---

## Project Structure

```text
.
├── query_engine.c
├── README.md
```

---

## Conclusion

This project demonstrates that columnar database workloads are highly suitable for OpenMP parallelization. Significant speedups can be achieved with relatively small modifications to the sequential implementation.

The experiments also highlight an important lesson in parallel programming:

> **Effective scheduling and hardware limitations often matter just as much as thread count.**

---

## Author

**Enes Recepoğlu**

Software Engineering Department

Parallel Programming Final Project

June 2026
