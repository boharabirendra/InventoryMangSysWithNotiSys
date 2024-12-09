# Inventory Management System

This project is an Inventory Management System that manages products, suppliers, and stock levels. It notifies suppliers when products need to be restocked and allows users to perform operations like viewing, selling, and checking product inventory.

## Features

- **Products**: Manage product details like name, category, price, stock level, and reorder threshold.
- **Suppliers**: Two types of suppliers are supported:
  - **Local Supplier**: Handles "Electronics" products with immediate restocking capability.
  - **Global Supplier**: Handles non-electronic products with slower restocking processes.
- **Notifications**: Notify suppliers when stock levels are low.
- **Sales**: Sell products and update stock levels dynamically.
- **Inventory Check**: View current inventory and low-stock alerts.

## Design Choices

1. **Class-Based Structure**:
   - Classes for `Product`, `Supplier`, `Organization`, `LocalSupplier`, and `GlobalSupplier`.
   - Ensures modularity and clear responsibility for each component.
2. **Dynamic Product Assignment**:
   - Suppliers are automatically assigned products based on their categories.
3. **Real-Time Notifications**:
   - Suppliers are notified only when stock reaches the reorder threshold.
4. **Menu-Driven Interface**:
   - Interactive CLI for supplier and product management.

## Instructions to Run the System

1. Clone or download the project files to your local system.
2. Open a terminal and navigate to the project folder.
3. Compile the code using the following command:
   ```bash
   g++ -o inventory_system main.cpp
   ```
