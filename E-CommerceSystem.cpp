#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <limits>

using namespace std;

struct Product {
    int id;
    string name;
    double price;
    int stock;
};

struct CartItem {
    int productId;
    int quantity;
};

class Store {
    vector<Product> products;
    vector<CartItem> cart;

    Product* findProduct(int id) {
        for (auto& p : products)
            if (p.id == id) return &p;
        return nullptr;
    }

public:
    Store() {
        products = {
            {1, "Laptop",      55000.00, 5},
            {2, "Smartphone",  20000.00, 10},
            {3, "Headphones",   1500.00, 25},
            {4, "Keyboard",      800.00, 30},
            {5, "Mouse",         500.00, 40}
        };
    }

    void browseProducts() const {
        cout << "\n=== Products ===\n";
        cout << left << setw(5) << "ID" << setw(15) << "Name"
             << setw(12) << "Price" << "Stock\n";
        cout << string(40, '-') << "\n";
        for (const auto& p : products) {
            cout << left << setw(5) << p.id << setw(15) << p.name
                 << setw(12) << fixed << setprecision(2) << p.price
                 << p.stock << "\n";
        }
    }

    void addToCart(int id, int qty) {
        Product* p = findProduct(id);
        if (!p) { cout << "Product not found.\n"; return; }
        if (qty <= 0) { cout << "Quantity must be positive.\n"; return; }

        // Account for quantity already in the cart
        int inCart = 0;
        for (auto& item : cart)
            if (item.productId == id) inCart = item.quantity;

        if (inCart + qty > p->stock) {
            cout << "Not enough stock. Available: " << p->stock - inCart << "\n";
            return;
        }

        for (auto& item : cart) {
            if (item.productId == id) {
                item.quantity += qty;
                cout << "Updated cart: " << p->name << " x" << item.quantity << "\n";
                return;
            }
        }
        cart.push_back({id, qty});
        cout << "Added to cart: " << p->name << " x" << qty << "\n";
    }

    void removeFromCart(int id) {
        for (size_t i = 0; i < cart.size(); ++i) {
            if (cart[i].productId == id) {
                cart.erase(cart.begin() + i);
                cout << "Item removed from cart.\n";
                return;
            }
        }
        cout << "Item not in cart.\n";
    }

    double cartTotal() {
        double total = 0;
        for (auto& item : cart)
            total += findProduct(item.productId)->price * item.quantity;
        return total;
    }

    void viewCart() {
        if (cart.empty()) { cout << "\nYour cart is empty.\n"; return; }
        cout << "\n=== Your Cart ===\n";
        cout << left << setw(15) << "Name" << setw(6) << "Qty"
             << setw(12) << "Price" << "Subtotal\n";
        cout << string(45, '-') << "\n";
        for (auto& item : cart) {
            Product* p = findProduct(item.productId);
            cout << left << setw(15) << p->name << setw(6) << item.quantity
                 << setw(12) << fixed << setprecision(2) << p->price
                 << p->price * item.quantity << "\n";
        }
        cout << string(45, '-') << "\n";
        cout << "Total: " << fixed << setprecision(2) << cartTotal() << "\n";
    }

    void checkout() {
        if (cart.empty()) { cout << "\nCart is empty. Nothing to purchase.\n"; return; }
        viewCart();

        cout << "\nConfirm purchase? (y/n): ";
        char c;
        cin >> c;
        if (c != 'y' && c != 'Y') { cout << "Purchase cancelled.\n"; return; }

        double total = cartTotal();
        for (auto& item : cart)
            findProduct(item.productId)->stock -= item.quantity;  // reduce stock
        cart.clear();

        cout << "\nPurchase complete! You paid "
             << fixed << setprecision(2) << total << ". Thank you!\n";
    }
};

int readInt(const string& prompt) {
    int x;
    cout << prompt;
    while (!(cin >> x)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. " << prompt;
    }
    return x;
}

int main() {
    Store store;
    int choice;

    do {
        cout << "\n===== E-COMMERCE MENU =====\n"
             << "1. Browse products\n"
             << "2. Add to cart\n"
             << "3. View cart\n"
             << "4. Remove from cart\n"
             << "5. Checkout\n"
             << "0. Exit\n";
        choice = readInt("Choose: ");

        switch (choice) {
            case 1: store.browseProducts(); break;
            case 2: {
                int id  = readInt("Product ID: ");
                int qty = readInt("Quantity: ");
                store.addToCart(id, qty);
                break;
            }
            case 3: store.viewCart(); break;
            case 4: store.removeFromCart(readInt("Product ID to remove: ")); break;
            case 5: store.checkout(); break;
            case 0: cout << "Goodbye!\n"; break;
            default: cout << "Invalid option.\n";
        }
    } while (choice != 0);

    return 0;
}