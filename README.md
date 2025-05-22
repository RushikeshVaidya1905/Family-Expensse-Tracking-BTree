# Family-Expensse-Tracking-BTree
Family Expense Tracking System
The Family Expense Tracking System efficiently manages and analyzes the expenses of each and every family member. It supports individual and family-level financial tracking, offering deep insights into category-wise spending, individual contributions, and monthly trends.

This application is entirely implemented using a B-tree data structure, which ensures fast and balanced operations for large sets of users, families, and expenses. All data — including users, families, and categorized expenses — is organized and stored in B-trees, allowing quick insertion, deletion, search, and sorted traversal.

Features

🧑‍💼 User Management

AddUser()

Inserts a new user into the User B-tree, maintaining sorted order by User ID for efficient lookups and updates.

👨‍👩‍👧‍👦 Family Management

CreateFamily()

Forms a family with 1 to 4 members, each linked through the User B-tree nodes.
Automatically computes total monthly family income and expenses.

Update_or_delete_individual_Family_details()

Update individual user or family information stored within the B-tree.
Deleting the last remaining member of a family triggers automatic deletion of the family node from the Family B-tree.

💰 Expense Management

AddExpense()

Adds an expense record into the Expense B-tree, categorized as:
Rent, Utility, Grocery, Stationary, or Leisure.
Expenses are indexed by User ID and Expense ID for quick and sorted access.

Update_delete_expense()

Allows modifications or deletion of existing expenses, with automatic recalculation of relevant totals across the B-tree.

📊 Expense Tracking and Analysis

Get_total_expense()

Traverses the Expense B-tree to calculate total family spending for the month.
Compares it against the total income and provides the difference and status (within/exceeding income).

Get_categorical_expense(string category)

Aggregates category-specific expenses using B-tree traversal.
Displays individual contributions in sorted descending order by amount.

Get_highest_expense_day()

Analyzes expense entries across the B-tree to identify the day with the highest spending.

Get_individual_expense(UserID)

Fetches all expenses of a given user from the B-tree, calculates the total, and provides a category-wise breakdown in descending order.
