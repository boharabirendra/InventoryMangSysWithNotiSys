#include <limits>
#include <vector>
#include <string>
#include <memory>
#include <iomanip>
#include <iostream>
#include <algorithm>

using namespace std;

class Organization;

class Supplier
{
protected:
    string name;
    vector<int> assignedProductIDs;

public:
    Supplier(const string &supplierName) : name(supplierName) {}
    virtual ~Supplier() = default;

    virtual void notifyRestock(int productID, int quantity) = 0;
    virtual void subscribeToOrganization(Organization *org) = 0;
    virtual void assignProduct(int productID) { assignedProductIDs.push_back(productID); }
    virtual vector<int> &getAssignedProducts() { return assignedProductIDs; }

    string getName() const { return name; }
};

class Product
{
private:
    int productID;
    string productName;
    string category;
    double price;
    int stockLevel;
    int reorderThreshold;

public:
    Product(int id, const string &name, const string &cat,
            double p, int stock, int threshold)
        : productID(id), productName(name), category(cat),
          price(p), stockLevel(stock), reorderThreshold(threshold) {}

    int getProductID() const { return productID; }
    string getProductName() const { return productName; }
    string getCategory() const { return category; }
    double getPrice() const { return price; }
    int getStockLevel() const { return stockLevel; }
    int getReorderThreshold() const { return reorderThreshold; }

    void updateStockLevel(int soldQuantity)
    {
        if (soldQuantity > stockLevel)
        {
            stockLevel = 0;
        }
        else
        {
            stockLevel -= soldQuantity;
        }
    }

    bool needsRestocking() const
    {
        return stockLevel <= reorderThreshold;
    }

    void setStockLevel(int newStockLevel)
    {
        stockLevel = newStockLevel;
    }

    void displayProductDetails() const
    {
        cout << left
             << setw(10) << productID
             << setw(20) << productName
             << setw(15) << category
             << setw(10) << price
             << setw(10) << stockLevel
             << endl;
    }
};

class Organization
{
private:
    vector<Product> products;
    vector<Supplier *> subscribedSuppliers;

public:
    void addProduct(const Product &product)
    {
        products.push_back(product);
    }

    void removeProduct(int productID)
    {
        auto it = remove_if(products.begin(), products.end(),
                            [productID](const Product &p)
                            { return p.getProductID() == productID; });
        products.erase(it, products.end());
    }

    Product *findProductByID(int productID)
    {
        auto it = find_if(products.begin(), products.end(),
                          [productID](const Product &p)
                          { return p.getProductID() == productID; });
        return (it != products.end()) ? &(*it) : nullptr;
    }

    vector<Product> &getAllProducts()
    {
        return products;
    }

    void displayProductsForSupplier(Supplier *supplier)
    {
        system("clear");
        cout << endl
             << "Products for " << supplier->getName() << endl;
        cout << endl
             << string(60, '-') << endl;
        cout << left
             << setw(10) << "ID"
             << setw(20) << "Product Name"
             << setw(15) << "Category"
             << setw(10) << "Price"
             << setw(10) << "Stock"
             << endl;
        cout << string(60, '-') << endl;

        for (auto &productID : supplier->getAssignedProducts())
        {
            Product *product = findProductByID(productID);
            if (product)
            {
                product->displayProductDetails();
            }
        }
    }

    void displayInventory() const
    {
        cout << endl
             << " ----------------" << endl;
        cout
            << "Current Inventory" << endl;
        cout << " ----------------" << endl;
        cout << left
             << setw(10) << "ID"
             << setw(20) << "Product Name"
             << setw(15) << "Category"
             << setw(10) << "Price"
             << setw(10) << "Stock"
             << endl;
        cout << string(65, '-') << endl;

        for (const auto &product : products)
        {
            product.displayProductDetails();
        }
    }

    void subscribeSupplier(Supplier *supplier)
    {
        subscribedSuppliers.push_back(supplier);
    }

    void checkInventoryAndNotify()
    {
        bool needRestocking = false;
        for (auto &product : products)
        {
            if (product.needsRestocking())
            {
                needRestocking = true;
                int quantityToRestock = product.getReorderThreshold() * 2 - product.getStockLevel();

                cout << endl
                     << "Low stock alert for: " << product.getProductName()
                     << " (ID: " << product.getProductID() << ")" << endl;

                for (auto *supplier : subscribedSuppliers)
                {
                    supplier->notifyRestock(product.getProductID(), quantityToRestock);
                }
            }
        }

        if (!needRestocking)
        {
            cout << endl
                 << "All products are well-stocked. No restocking needed." << endl;
        }
    }

    void sendTargetedNotification(Product *product, Supplier *supplier)
    {
        if (product->needsRestocking())
        {
            /**
             * For demo purpose restocking quantity is as same as of sold quantity
             */
            int quantityToRestock = product->getReorderThreshold() * 2 - product->getStockLevel();

            cout << endl
                 << "-----------------------------" << endl;
            cout << "Targeted Restock Notification " << endl;
            cout << "-----------------------------" << endl;
            cout << "Low stock alert for: " << product->getProductName()
                 << " (ID: " << product->getProductID() << ")" << endl;
            supplier->notifyRestock(product->getProductID(), quantityToRestock);
        }
    }
};

class LocalSupplier : public Supplier
{
private:
    Organization *subscribedOrg;

public:
    LocalSupplier(const string &name) : Supplier(name), subscribedOrg(nullptr) {}

    void notifyRestock(int productID, int quantity) override
    {
        cout << "Local Supplier " << name
             << " received restocking request for Product ID: "
             << productID << ", Quantity: " << quantity << endl;
        cout << "Local Supplier will restock immediately!\n";
    }

    void subscribeToOrganization(Organization *org) override
    {
        subscribedOrg = org;
        org->subscribeSupplier(this);

        vector<Product> &products = org->getAllProducts();
        for (auto &product : products)
        {
            if (product.getCategory() == "Electronics")
            {
                assignProduct(product.getProductID());
            }
        }
    }
};

class GlobalSupplier : public Supplier
{
private:
    Organization *subscribedOrg;

public:
    GlobalSupplier(const string &name) : Supplier(name), subscribedOrg(nullptr) {}

    void notifyRestock(int productID, int quantity) override
    {
        cout << "Global Supplier " << name
             << " received restocking request for Product ID: "
             << productID << ", Quantity: " << quantity << endl;
        cout << "Global Supplier will process restocking (may take longer).\n";
    }

    void subscribeToOrganization(Organization *org) override
    {
        subscribedOrg = org;
        org->subscribeSupplier(this);

        vector<Product> &products = org->getAllProducts();
        for (auto &product : products)
        {
            if (product.getCategory() != "Electronics")
            {
                assignProduct(product.getProductID());
            }
        }
    }
};

void displaySupplierMenu()
{
    cout << endl
         << "------------------" << endl;
    cout << "Supplier Selection" << endl;
    cout << "------------------" << endl;
    cout << "1. Local Supplier " << endl;
    cout << "2. Global Supplier" << endl;
    cout << "3. Exit" << endl;
    cout << endl
         << "Enter your choice: ";
}

void displayProductMenu()
{
    cout << endl
         << "------------" << endl;
    cout << "Product Menu" << endl;
    cout << "------------" << endl;
    cout << "1. View Assigned Products" << endl;
    cout << "2. Sell Product" << endl;
    cout << "3. Check Inventory Status" << endl;
    cout << "4. Return to Supplier Menu" << endl;
    cout << endl
         << "Enter your choice: ";
}

int main()
{
    system("clear");

    Product laptop(101, "Laptop", "Electronics", 1000.0, 10, 5);
    Product smartphone(102, "Smartphone", "Electronics", 500.0, 10, 5);
    Product tablet(103, "Tablet", "Electronics", 300.0, 7, 4);
    Product refrigerator(104, "Refrigerator", "Appliances", 500.0, 5, 3);
    Product microwave(105, "Microwave", "Appliances", 200.0, 8, 4);

    Organization organization;

    organization.addProduct(laptop);
    organization.addProduct(smartphone);
    organization.addProduct(tablet);
    organization.addProduct(refrigerator);
    organization.addProduct(microwave);

    LocalSupplier localSupplier("Local Supplier");
    GlobalSupplier globalSupplier("Global Supplier");

    localSupplier.subscribeToOrganization(&organization);
    globalSupplier.subscribeToOrganization(&organization);

    int supplierChoice, productChoice;
    Supplier *currentSupplier = nullptr;

    while (true)
    {
        displaySupplierMenu();
        cin >> supplierChoice;

        if (supplierChoice == 3)
            break;

        switch (supplierChoice)
        {
        case 1:
            currentSupplier = &localSupplier;
            break;
        case 2:
            currentSupplier = &globalSupplier;
            break;
        default:
            cout << "Invalid choice" << endl;
            break;
        }

        while (true)
        {
            displayProductMenu();
            cin >> productChoice;

            switch (productChoice)
            {
            case 1:
            {
                organization.displayProductsForSupplier(currentSupplier);
                break;
            }
            case 2:
            {
                int productId, quantity;
                organization.displayProductsForSupplier(currentSupplier);

                cout << endl
                     << "Enter Product ID to sell: ";
                cin >> productId;

                Product *product = organization.findProductByID(productId);
                if (product)
                {
                    auto &assignedProducts = currentSupplier->getAssignedProducts();
                    auto it = find(assignedProducts.begin(), assignedProducts.end(), productId);

                    if (it == assignedProducts.end())
                    {
                        cout << "This product is not assigned to the current supplier." << endl;
                        break;
                    }

                    cout << "Enter quantity to sell: ";
                    cin >> quantity;

                    if (quantity > 0 && quantity <= product->getStockLevel())
                    {
                        int originalStockLevel = product->getStockLevel();

                        product->updateStockLevel(quantity);

                        cout << endl
                             << "Sale successful! Remaining stock: "
                             << product->getStockLevel() << endl;

                        if (product->needsRestocking())
                        {
                            organization.sendTargetedNotification(product, currentSupplier);
                        }
                    }
                    else
                    {
                        cout << "Invalid quantity or insufficient stock." << endl;
                    }
                }
                else
                {
                    cout << "Product not found." << endl;
                }
                break;
            }
            case 3:
            {
                organization.checkInventoryAndNotify();
                break;
            }
            case 4:
                goto supplierMenuLabel;
            default:
                cout << "Invalid choice" << endl;
                break;
            }
        }
    supplierMenuLabel:;
    }

    return 0;
}
