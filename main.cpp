#include <algorithm>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <queue>
#include <sstream>
#include <string>
#include <unordered_map>
#include <regex>
#include <limits>
#include "CustomHashTable.h"
using namespace std;
namespace fs = std::filesystem;

// Product class
class Product {
public:
    string id, name, category, subcategory;
    double price;
    int quantity;

    Product(string _id = "", string _name = "", string _category = "",
            string _subcategory = "", double _price = 0.0, int _quantity = 0)
        : id(_id), name(_name), category(_category), subcategory(_subcategory),
          price(_price), quantity(_quantity) {}

    string toString() const {
        return "ID: " + id + ", Name: " + name + ", Category: " + category + " - " +
               subcategory + ", Price: $" + to_string(price) +
               ", Stock: " + to_string(quantity);
    }

    bool operator<(const Product& other) const { return id < other.id; }
    bool operator==(const Product& other) const { return id == other.id; }
};

// AVL Tree Node for Products
struct AVLNode {
    Product data;
    AVLNode* left;
    AVLNode* right;
    int height;
    AVLNode(const Product& _data)
        : data(_data), left(nullptr), right(nullptr), height(1) {}
};

// ProductAVLTree class
class ProductAVLTree {
private:
    AVLNode* root;

    int getHeight(AVLNode* node) const { return node ? node->height : 0; }

    int getBalance(AVLNode* node) const {
        return node ? getHeight(node->left) - getHeight(node->right) : 0;
    }

    AVLNode* rightRotate(AVLNode* y) {
        AVLNode* x = y->left;
        AVLNode* T2 = x->right;
        x->right = y;
        y->left = T2;
        y->height = max(getHeight(y->left), getHeight(y->right)) + 1;
        x->height = max(getHeight(x->left), getHeight(x->right)) + 1;
        return x;
    }

    AVLNode* leftRotate(AVLNode* x) {
        AVLNode* y = x->right;
        AVLNode* T2 = y->left;
        y->left = x;
        x->right = T2;
        x->height = max(getHeight(x->left), getHeight(x->right)) + 1;
        y->height = max(getHeight(y->left), getHeight(y->right)) + 1;
        return y;
    }

    AVLNode* insertNode(AVLNode* node, const Product& product) {
        if (!node)
            return new AVLNode(product);

        if (product.id < node->data.id)
            node->left = insertNode(node->left, product);
        else if (product.id > node->data.id)
            node->right = insertNode(node->right, product);
        else {
            node->data = product; // Update existing product
            return node;
        }

        node->height = max(getHeight(node->left), getHeight(node->right)) + 1;
        int balance = getBalance(node);

        if (balance > 1 && product.id < node->left->data.id)
            return rightRotate(node);
        if (balance < -1 && product.id > node->right->data.id)
            return leftRotate(node);
        if (balance > 1 && product.id > node->left->data.id) {
            node->left = leftRotate(node->left);
            return rightRotate(node);
        }
        if (balance < -1 && product.id < node->right->data.id) {
            node->right = rightRotate(node->right);
            return leftRotate(node);
        }
        return node;
    }

    AVLNode* minValueNode(AVLNode* node) {
        AVLNode* current = node;
        while (current->left)
            current = current->left;
        return current;
    }

    AVLNode* removeNode(AVLNode* node, const string& id) {
        if (!node)
            return node;

        if (id < node->data.id)
            node->left = removeNode(node->left, id);
        else if (id > node->data.id)
            node->right = removeNode(node->right, id);
        else {
            if (!node->left || !node->right) {
                AVLNode* temp = node->left ? node->left : node->right;
                if (!temp) {
                    temp = node;
                    node = nullptr;
                } else {
                    *node = *temp;
                }
                delete temp;
            } else {
                AVLNode* temp = minValueNode(node->right);
                node->data = temp->data;
                node->right = removeNode(node->right, temp->data.id);
            }
        }

        if (!node)
            return node;

        node->height = max(getHeight(node->left), getHeight(node->right)) + 1;
        int balance = getBalance(node);

        if (balance > 1 && getBalance(node->left) >= 0)
            return rightRotate(node);
        if (balance > 1 && getBalance(node->left) < 0) {
            node->left = leftRotate(node->left);
            return rightRotate(node);
        }
        if (balance < -1 && getBalance(node->right) <= 0)
            return leftRotate(node);
        if (balance < -1 && getBalance(node->right) > 0) {
            node->right = rightRotate(node->right);
            return leftRotate(node);
        }
        return node;
    }

    AVLNode* findNode(AVLNode* node, const string& id) const {
        if (!node || node->data.id == id)
            return node;
        if (id < node->data.id)
            return findNode(node->left, id);
        return findNode(node->right, id);
    }

    void inOrder(AVLNode* node, vector<Product>& result) const {
        if (node) {
            inOrder(node->left, result);
            result.push_back(node->data);
            inOrder(node->right, result);
        }
    }

    void deleteTree(AVLNode* node) {
        if (node) {
            deleteTree(node->left);
            deleteTree(node->right);
            delete node;
        }
    }

public:
    ProductAVLTree() : root(nullptr) {}
    ~ProductAVLTree() { deleteTree(root); }

    void insert(const Product& product) { root = insertNode(root, product); }

    bool remove(const string& id) {
        AVLNode* node = findNode(root, id);
        if (!node)
            return false;
        root = removeNode(root, id);
        return true;
    }

    Product* find(const string& id) {
        AVLNode* node = findNode(root, id);
        return node ? &node->data : nullptr;
    }

    vector<Product> getAllProducts() const {
        vector<Product> result;
        inOrder(root, result);
        return result;
    }
};

// Linked List Node (generic)
template <typename T>
struct Node {
    T data;
    Node* next;
    Node(const T& _data) : data(_data), next(nullptr) {}
};

// Linked List (generic)
template <typename T>
class LinkedList {
private:
    Node<T>* head;
    size_t size;

public:
    LinkedList() : head(nullptr), size(0) {}
    ~LinkedList() { clear(); }

    void push_back(const T& item) {
        Node<T>* newNode = new Node<T>(item);
        if (!head) {
            head = newNode;
        } else {
            Node<T>* curr = head;
            while (curr->next)
                curr = curr->next;
            curr->next = newNode;
        }
        size++;
    }

    bool remove(const T& item) {
        Node<T>* curr = head;
        Node<T>* prev = nullptr;
        while (curr) {
            if (curr->data == item) {
                if (prev) {
                    prev->next = curr->next;
                } else {
                    head = curr->next;
                }
                delete curr;
                size--;
                return true;
            }
            prev = curr;
            curr = curr->next;
        }
        return false;
    }

    Node<T>* find(const string& id) const {
        Node<T>* curr = head;
        while (curr) {
            if constexpr (is_same_v<T, CartItem>) {
                if (curr->data.product.id == id)
                    return curr;
            } else if constexpr (is_same_v<T, pair<Product, int>>) {
                if (curr->data.first.id == id)
                    return curr;
            }
            curr = curr->next;
        }
        return nullptr;
    }

    Node<T>* begin() const { return head; }
    size_t getSize() const { return size; }

    void clear() {
        Node<T>* curr = head;
        while (curr) {
            Node<T>* next = curr->next;
            delete curr;
            curr = next;
        }
        head = nullptr;
        size = 0;
    }
};

// Cart Item structure
struct CartItem {
    Product product;
    int quantity;

    CartItem(const Product& _product, int _quantity)
        : product(_product), quantity(_quantity) {}
    bool operator==(const CartItem& other) const {
        return product.id == other.product.id;
    }
};

// Order class
class Order {
public:
    string orderId, trackingId, timestamp, customerName, customerAddress,
           customerPhone, paymentMethod;
    LinkedList<pair<Product, int>> items;
    double totalPrice;

    Order(string _orderId = "", string _trackingId = "", string _timestamp = "",
          string _customerName = "", string _customerAddress = "",
          string _customerPhone = "", string _paymentMethod = "",
          const LinkedList<pair<Product, int>>& _items = LinkedList<pair<Product, int>>(),
          double _totalPrice = 0.0)
        : orderId(_orderId), trackingId(_trackingId), timestamp(_timestamp),
          customerName(_customerName), customerAddress(_customerAddress),
          customerPhone(_customerPhone), paymentMethod(_paymentMethod),
          items(_items), totalPrice(_totalPrice) {}

    string toString() const {
        stringstream ss;
        ss << "Order ID: " << orderId << "\nTracking ID: " << trackingId
           << "\nTimestamp: " << timestamp << "\nCustomer: " << customerName << ", "
           << customerAddress << ", " << customerPhone
           << "\nPayment Method: " << paymentMethod << "\nItems:\n";
        for (Node<pair<Product, int>>* node = items.begin(); node;
             node = node->next) {
            const auto& item = node->data;
            ss << item.first.name << " x " << item.second << " = $"
               << (item.first.price * item.second) << "\n";
        }
        ss << "Total: $" << totalPrice;
        return ss.str();
    }
};

// Customer class
class Customer {
public:
    string id, name, email;
    Customer(string _id = "", string _name = "", string _email = "")
        : id(_id), name(_name), email(_email) {}
    string toString() const {
        return "ID: " + id + ", Name: " + name + ", Email: " + email;
    }
};

// Specialization for Customer value type
template <>
void CustomHashTable<string, Customer>::save(const string& filename) const {
    ofstream ofs(filename);
    if (ofs.is_open()) {
        for (const auto& bucket : table) {
            for (const auto& node : bucket) {
                ofs << node.first << "," << node.second.name << "," << node.second.email << "\n";
            }
        }
        ofs.close();
    } else {
        cerr << "Error saving to " << filename << endl;
    }
}

template <>
void CustomHashTable<string, Customer>::load(const string& filename) {
    ifstream ifs(filename);
    if (ifs.is_open()) {
        string line;
        while (getline(ifs, line)) {
            stringstream ss(line);
            string id, name, email;
            getline(ss, id, ',');
            getline(ss, name, ',');
            getline(ss, email);
            insert(id, Customer(id, name, email));
        }
        ifs.close();
    }
}

// Customer Hash Table
class CustomerHashTable {
private:
    CustomHashTable<string, Customer> table;
    const string CUSTOMERS_FILE = "wearhouse/customers.txt";

public:
    CustomerHashTable() : table(100) {}

    void insert(const Customer& customer) {
        table.insert(customer.id, customer);
    }

    bool remove(const string& id) {
        return table.remove(id);
    }

    Customer* find(const string& id) {
        return const_cast<Customer*>(table.find(id)); // Safe cast
    }

    vector<Customer> getAllCustomers() const {
        return table.getAll();
    }

    bool isEmpty() const {
        return table.isEmpty();
    }

    void save() const {
        table.save(CUSTOMERS_FILE);
        cout << "Customers saved successfully." << endl;
    }

    void load() {
        table.load(CUSTOMERS_FILE);
    }
};

// Sales Hash Table
class SalesHashTable {
private:
    CustomHashTable<string, double> table;
    const string SALES_FILE = "wearhouse/sales.txt";

public:
    SalesHashTable() : table(100) {}

    void insert(const string& monthYear, double amount) {
        double* existing = const_cast<double*>(table.find(monthYear)); // Safe cast
        if (existing) {
            *existing += amount;
            table.insert(monthYear, *existing);
        } else {
            table.insert(monthYear, amount);
        }
    }

    double get(const string& monthYear) const {
        const double* value = table.find(monthYear); // Now works with const
        return value ? *value : 0.0;
    }

    void save() const {
        table.save(SALES_FILE);
    }

    void load() {
        table.load(SALES_FILE);
    }
};

// Cart class
class Cart {
private:
    LinkedList<CartItem> items;

public:
    Cart() {}

    void addProduct(const Product& product, int quantity) {
        Node<CartItem>* node = items.find(product.id);
        if (node) {
            node->data.quantity += quantity;
        } else {
            items.push_back(CartItem(product, quantity));
        }
    }

    const LinkedList<CartItem>& getItems() const { return items; }

    double getTotalPrice() const {
        double total = 0.0;
        for (Node<CartItem>* node = items.begin(); node; node = node->next) {
            total += node->data.product.price * node->data.quantity;
        }
        return total;
    }

    void displayCart() const {
        if (items.getSize() == 0) {
            cout << "Cart is empty." << endl;
            return;
        }
        cout << "\n--- Cart Contents ---" << endl;
        for (Node<CartItem>* node = items.begin(); node; node = node->next) {
            const auto& item = node->data;
            cout << item.product.name << " x " << item.quantity << " = $"
                 << (item.product.price * item.quantity) << endl;
        }
        cout << "Total: $" << getTotalPrice() << endl;
    }

    void clearCart() { items.clear(); }
};

// Comparator for priority_queue
struct OrderComparator {
    bool operator()(const Order& a, const Order& b) const {
        return a.totalPrice < b.totalPrice; // Max-heap: higher totalPrice first
    }
};

// Admin Hash Table
class AdminHashTable {
private:
    CustomHashTable<string, string> adminHashTable;
    const string ADMIN_FILE = "wearhouse/admins.txt";

    string hashPassword(const string& password) {
        hash<string> hasher;
        return to_string(hasher(password));
    }

    bool isValidUsername(const string& username) {
        regex pattern("^[a-zA-Z0-9_]{4,20}$");
        return regex_match(username, pattern);
    }

    bool isValidPassword(const string& password) {
        regex pattern("^(?=.*[A-Z])(?=.*[a-z])(?=.*[0-9])(?=.*[!@#$%^&*])[A-Za-z0-9!@#$%^&*]{8,}$");
        return regex_match(password, pattern);
    }

public:
    AdminHashTable() : adminHashTable(100) { loadAdmins(); }

    bool authenticate(const string& username, const string& password) {
        string* hashedPassword = const_cast<string*>(adminHashTable.find(username));
        if (hashedPassword) {
            return *hashedPassword == hashPassword(password);
        }
        return false;
    }

    bool addAdmin(const string& username, const string& password, const string& confirmPassword) {
        if (adminHashTable.find(username)) {
            cout << "Username already exists. Choose a different username." << endl;
            return false;
        }
        if (!isValidUsername(username)) {
            cout << "Username must be 4–20 characters long and contain only letters, numbers, or underscores." << endl;
            return false;
        }
        if (!isValidPassword(password)) {
            cout << "Password must be at least 8 characters long and include an uppercase letter, lowercase letter, digit, and special character." << endl;
            return false;
        }
        if (password != confirmPassword) {
            cout << "Passwords do not match." << endl;
            return false;
        }

        adminHashTable.insert(username, hashPassword(password));
        saveAdmins();
        cout << "Admin added successfully." << endl;
        return true;
    }

    void saveAdmins() const {
        adminHashTable.save(ADMIN_FILE);
    }

    void loadAdmins() {
        if (!fs::exists(ADMIN_FILE)) {
            adminHashTable.insert("admin", hashPassword("Admin@123"));
            saveAdmins();
            return;
        }
        adminHashTable.load(ADMIN_FILE);
    }

    bool isEmpty() const { return adminHashTable.isEmpty(); }
};

// FaminEcommerce class
class FaminEcommerce {
private:
    ProductAVLTree products;
    priority_queue<Order, vector<Order>, OrderComparator> orders;
    CustomerHashTable customers;
    SalesHashTable monthlySales;
    AdminHashTable adminTable;
    Cart cart;
    const string PRODUCTS_FILE = "wearhouse/products.txt";
    const string ORDERS_FILE = "wearhouse/orders.txt";
    const string CUSTOMERS_FILE = "wearhouse/customers.txt";
    const string SALES_FILE = "wearhouse/sales.txt";
    const string SHIPMENTS_FILE = "wearhouse/database/shipments.txt";
    static unsigned long nextOrderId;
    static unsigned long nextTrackingId;
    const string ID_COUNTERS_FILE = "wearhouse/id_counters.txt";

    bool isDirectoryWritable(const string& dirPath) const {
        try {
            if (!fs::exists(dirPath))
                return false;
            string testFile = dirPath + "/.test_writable";
            ofstream ofs(testFile);
            if (!ofs.is_open())
                return false;
            ofs.close();
            fs::remove(testFile);
            return true;
        } catch (const fs::filesystem_error& e) {
            return false;
        }
    }

    bool ensureDirectoriesExist() {
        try {
            fs::create_directories("wearhouse/database");
            if (!isDirectoryWritable("wearhouse")) {
                cerr << "Error: Directory 'wearhouse' is not writable. Please check permissions." << endl;
                return false;
            }
            if (!isDirectoryWritable("wearhouse/database")) {
                cerr << "Error: Directory 'wearhouse/database' is not writable. Please check permissions." << endl;
                return false;
            }
            return true;
        } catch (const fs::filesystem_error& e) {
            cerr << "Error creating directories: " << e.what() << "\nPlease check permissions." << endl;
            return false;
        }
    }

    void loadIdCounters() {
        if (!fs::exists(ID_COUNTERS_FILE))
            return;
        ifstream ifs(ID_COUNTERS_FILE);
        if (ifs.is_open()) {
            string line;
            if (getline(ifs, line)) {
                stringstream ss(line);
                ss >> nextOrderId >> nextTrackingId;
            }
            ifs.close();
        } else {
            cerr << "Warning: Could not open " << ID_COUNTERS_FILE << endl;
        }
    }

    void saveIdCounters() const {
        ofstream ofs(ID_COUNTERS_FILE);
        if (ofs.is_open()) {
            ofs << nextOrderId << " " << nextTrackingId << "\n";
            ofs.close();
        } else {
            cerr << "Error saving ID counters to " << ID_COUNTERS_FILE << endl;
        }
    }

    string generateOrderId() {
        string currentId = to_string(nextOrderId);
        string id = "ORD" + string(6 - currentId.length(), '0') + currentId;
        nextOrderId++;
        saveIdCounters();
        return id;
    }

    string generateTrackingId() {
        string currentId = to_string(nextTrackingId);
        string id = "TRK" + string(6 - currentId.length(), '0') + currentId;
        nextTrackingId++;
        saveIdCounters();
        return id;
    }

    void loadProducts() {
        if (fs::exists(PRODUCTS_FILE)) {
            ifstream ifs(PRODUCTS_FILE);
            if (ifs.is_open()) {
                string line;
                while (getline(ifs, line)) {
                    stringstream ss(line);
                    string id, name, category, subcategory;
                    double price;
                    int quantity;
                    getline(ss, id, ',');
                    getline(ss, name, ',');
                    getline(ss, category, ',');
                    getline(ss, subcategory, ',');
                    ss >> price;
                    ss.ignore();
                    ss >> quantity;
                    products.insert(Product(id, name, category, subcategory, price, quantity));
                }
                ifs.close();
            } else {
                cerr << "Warning: Could not open " << PRODUCTS_FILE << endl;
            }
        }
        if (!products.find("1")) {
            products.insert(Product("1", "Lablis", "Women", "Eid Edition", 25700.00, 10));
            products.insert(Product("2", "T-Shirt", "Men", "Casual", 1500.00, 20));
            saveProducts();
        }
    }

    void saveProducts() const {
        ofstream ofs(PRODUCTS_FILE);
        if (ofs.is_open()) {
            auto allProducts = products.getAllProducts();
            for (const auto& p : allProducts) {
                ofs << p.id << "," << p.name << "," << p.category << ","
                    << p.subcategory << "," << p.price << "," << p.quantity << "\n";
            }
            ofs.close();
            cout << "Products saved successfully." << endl;
        } else {
            cerr << "Error saving products to " << PRODUCTS_FILE << endl;
        }
    }

    void loadOrders() {
        if (!fs::exists(ORDERS_FILE))
            return;
        ifstream ifs(ORDERS_FILE);
        if (ifs.is_open()) {
            string line;
            while (getline(ifs, line)) {
                stringstream ss(line);
                string orderId, trackingId, timestamp, customerName, customerAddress,
                       customerPhone, paymentMethod, itemsStr;
                double totalPrice;
                getline(ss, orderId, ',');
                getline(ss, trackingId, ',');
                getline(ss, timestamp, ',');
                getline(ss, customerName, ',');
                getline(ss, customerAddress, ',');
                getline(ss, customerPhone, ',');
                getline(ss, paymentMethod, ',');
                ss >> totalPrice;
                ss.ignore();
                getline(ss, itemsStr);
                LinkedList<pair<Product, int>> orderItems;
                stringstream itemsSs(itemsStr);
                string item;
                while (getline(itemsSs, item, ';')) {
                    if (item.empty())
                        continue;
                    size_t colonPos = item.find(':');
                    if (colonPos == string::npos)
                        continue;
                    string productId = item.substr(0, colonPos);
                    int quantity;
                    try {
                        quantity = stoi(item.substr(colonPos + 1));
                    } catch (...) {
                        cerr << "Invalid quantity in order items: " << item << endl;
                        continue;
                    }
                    Product* product = products.find(productId);
                    if (product) {
                        orderItems.push_back({*product, quantity});
                    } else {
                        cerr << "Product ID " << productId << " not found for order " << orderId << endl;
                    }
                }
                orders.push(Order(orderId, trackingId, timestamp, customerName, customerAddress, customerPhone, paymentMethod, orderItems, totalPrice));
            }
            ifs.close();
        } else {
            cerr << "Warning: Could not open " << ORDERS_FILE << endl;
        }
    }

    void saveOrders() const {
        ofstream ofs(ORDERS_FILE);
        if (ofs.is_open()) {
            priority_queue<Order, vector<Order>, OrderComparator> temp = orders;
            while (!temp.empty()) {
                const Order& order = temp.top();
                ofs << order.orderId << "," << order.trackingId << ","
                    << order.timestamp << "," << order.customerName << ","
                    << order.customerAddress << "," << order.customerPhone << ","
                    << order.paymentMethod << "," << order.totalPrice;
                string itemsStr;
                for (Node<pair<Product, int>>* node = order.items.begin(); node;
                     node = node->next) {
                    if (!itemsStr.empty())
                        itemsStr += ";";
                    itemsStr += node->data.first.id + ":" + to_string(node->data.second);
                }
                ofs << "," << itemsStr << "\n";
                temp.pop();
            }
            ofs.close();
            cout << "Orders saved successfully." << endl;
        } else {
            cerr << "Error saving orders to " << ORDERS_FILE << endl;
        }
    }

    void loadCustomers() {
        customers.load();
    }

    void saveCustomers() const {
        customers.save();
    }

    void loadSales() {
        monthlySales.load();
    }

    void saveSales() const {
        monthlySales.save();
    }

    void displayProducts() const {
        auto allProducts = products.getAllProducts();
        if (allProducts.empty()) {
            cout << "No products available." << endl;
            return;
        }
        cout << "\n--- Products ---" << endl;
        for (const auto& p : allProducts) {
            cout << p.toString() << endl;
        }
    }

    void filterAndDisplayProducts(const string& category) const {
        auto allProducts = products.getAllProducts();
        bool found = false;
        cout << "\n--- " << category << " Products ---" << endl;
        for (const auto& p : allProducts) {
            if (p.category == category) {
                cout << p.toString() << endl;
                found = true;
            }
        }
        if (!found) {
            cout << "No products in category: " << category << endl;
        }
    }

    void addToCart(const string& productId, int quantity) {
        if (quantity <= 0) {
            cout << "Quantity must be positive." << endl;
            return;
        }
        Product* product = products.find(productId);
        if (!product) {
            cout << "Product ID " << productId << " not found." << endl;
        } else if (product->quantity < quantity) {
            cout << "Insufficient stock for " << product->name
                 << ". Available: " << product->quantity << endl;
        } else {
            cart.addProduct(*product, quantity);
            product->quantity -= quantity;
            products.insert(*product);
            saveProducts();
            cout << quantity << " x " << product->name << " added to cart." << endl;
        }
    }

    void placeOrder() {
        if (cart.getItems().getSize() == 0) {
            cout << "Cart is empty." << endl;
            return;
        }
        cout << "\nEnter customer details:\nName: ";
        string name, address, phone;
        cin.ignore();
        getline(cin, name);
        cout << "Address: ";
        getline(cin, address);
        cout << "Phone: ";
        getline(cin, phone);
        if (name.empty() || address.empty() || phone.empty()) {
            cout << "All customer details are required." << endl;
            return;
        }
        cout << "Select payment method (1: Cash, 2: Online Payment): ";
        int paymentChoice;
        if (!(cin >> paymentChoice) || (paymentChoice != 1 && paymentChoice != 2)) {
            cout << "Invalid payment method." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }
        cin.ignore();
        string paymentMethod = paymentChoice == 1 ? "Cash" : "Online Payment";
        string orderId = generateOrderId();
        string trackingId = generateTrackingId();
        time_t now = time(nullptr);
        char timestamp[20];
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));
        LinkedList<pair<Product, int>> orderItems;
        for (Node<CartItem>* node = cart.getItems().begin(); node; node = node->next) {
            orderItems.push_back({node->data.product, node->data.quantity});
        }
        Order order(orderId, trackingId, timestamp, name, address, phone, paymentMethod, orderItems, cart.getTotalPrice());
        orders.push(order);
        saveOrders();
        string monthYear = string(timestamp).substr(5, 5);
        monthlySales.insert(monthYear, cart.getTotalPrice());
        saveSales();
        appendShipment(orderId, trackingId, name, address, "in progress");
        cout << "\nOrder placed successfully!\nOrder ID: " << orderId
             << "\nTracking ID: " << trackingId << "\nTotal: $" << cart.getTotalPrice() << endl;
        cart.clearCart();
    }

    void appendShipment(const string& orderId, const string& trackingId,
                        const string& customerName, const string& address,
                        const string& status) {
        ofstream ofs(SHIPMENTS_FILE, ios::app);
        if (ofs.is_open()) {
            ofs << orderId << "," << trackingId << "," << customerName << ","
                << address << "," << status << "\n";
            ofs.close();
        } else {
            cerr << "Error writing to " << SHIPMENTS_FILE << endl;
        }
    }

    void listProducts() const { displayProducts(); }

    void addProduct() {
        cout << "\n--- Add Product ---" << endl;
        string id, name, category, subcategory, priceStr, quantityStr;
        cout << "Enter Product ID: ";
        cin.ignore();
        getline(cin, id);
        if (id.empty() || id.find(',') != string::npos) {
            cout << "Product ID cannot be empty or contain commas." << endl;
            return;
        }
        if (products.find(id)) {
            cout << "Product ID already exists." << endl;
            return;
        }
        cout << "Enter Product Name: ";
        getline(cin, name);
        cout << "Enter Category (Men/Women): ";
        getline(cin, category);
        cout << "Enter Subcategory (e.g., New In, Eid Edition): ";
        getline(cin, subcategory);
        cout << "Enter Price: ";
        getline(cin, priceStr);
        cout << "Enter Quantity: ";
        getline(cin, quantityStr);
        try {
            double price = stod(priceStr);
            int quantity = stoi(quantityStr);
            if (price < 0 || quantity < 0) {
                cout << "Price and quantity cannot be negative." << endl;
                return;
            }
            products.insert(Product(id, name, category, subcategory, price, quantity));
            saveProducts();
            cout << "Product added successfully." << endl;
        } catch (...) {
            cout << "Price and Quantity must be valid numbers." << endl;
        }
    }

    void editProduct() {
        cout << "\n--- Edit Product ---" << endl;
        string id;
        cout << "Enter Product ID to edit: ";
        cin.ignore();
        getline(cin, id);
        Product* product = products.find(id);
        if (!product) {
            cout << "Product ID not found." << endl;
            return;
        }
        cout << "Current Product: " << product->toString() << endl;
        string name, category, subcategory, priceStr, quantityStr;
        cout << "Enter new Product Name (or press Enter to keep current): ";
        getline(cin, name);
        cout << "Enter new Category (or press Enter to keep current): ";
        getline(cin, category);
        cout << "Enter new Subcategory (or press Enter to keep current): ";
        getline(cin, subcategory);
        cout << "Enter new Price (or press Enter to keep current): ";
        getline(cin, priceStr);
        cout << "Enter new Quantity (or press Enter to keep current): ";
        getline(cin, quantityStr);

        try {
            name = name.empty() ? product->name : name;
            category = category.empty() ? product->category : category;
            subcategory = subcategory.empty() ? product->subcategory : subcategory;
            double price = priceStr.empty() ? product->price : stod(priceStr);
            int quantity = quantityStr.empty() ? product->quantity : stoi(quantityStr);

            if (price < 0 || quantity < 0) {
                cout << "Price and quantity cannot be negative." << endl;
                return;
            }
            products.remove(id);
            products.insert(Product(id, name, category, subcategory, price, quantity));
            saveProducts();
            cout << "Product updated successfully." << endl;
        } catch (...) {
            cout << "Price and Quantity must be valid numbers." << endl;
        }
    }

    void deleteProduct() {
        cout << "\n--- Delete Product ---" << endl;
        string id;
        cout << "Enter Product ID to delete: ";
        cin.ignore();
        getline(cin, id);
        Product* product = products.find(id);
        if (!product) {
            cout << "Product ID not found." << endl;
            return;
        }
        cout << "Product to delete: " << product->toString() << endl;
        cout << "Confirm deletion (yes/no): ";
        string confirm;
        getline(cin, confirm);
        transform(confirm.begin(), confirm.end(), confirm.begin(), ::tolower);
        if (confirm == "yes") {
            products.remove(id);
            saveProducts();
            cout << "Product deleted successfully." << endl;
        } else {
            cout << "Deletion cancelled." << endl;
        }
    }

    Customer* findCustomer(const string& id) {
        return customers.find(id);
    }

    void findCustomerMenu() {
        cout << "\n--- Find Customer ---" << endl;
        string id;
        cout << "Enter Customer ID: ";
        cin.ignore();
        getline(cin, id);
        Customer* customer = findCustomer(id);
        if (customer) {
            cout << "Customer found: " << customer->toString() << endl;
        } else {
            cout << "Customer ID not found." << endl;
        }
    }

    void removeCustomer() {
        cout << "\n--- Remove Customer ---" << endl;
        string id;
        cout << "Enter Customer ID: ";
        cin.ignore();
        getline(cin, id);
        if (customers.remove(id)) {
            saveCustomers();
            cout << "Customer removed successfully." << endl;
        } else {
            cout << "Customer ID not found." << endl;
        }
    }

    void listOrders() const {
        if (orders.empty()) {
            cout << "No orders available." << endl;
            return;
        }
        cout << "\n--- Order List (Sorted by Total Price, Highest First) ---" << endl;
        priority_queue<Order, vector<Order>, OrderComparator> temp = orders;
        while (!temp.empty()) {
            cout << temp.top().toString() << "\n---" << endl;
            temp.pop();
        }
    }

    void addCustomer() {
        cout << "\n--- Add Customer ---" << endl;
        string id, name, email;
        cout << "Enter Customer ID: ";
        cin.ignore();
        getline(cin, id);
        if (id.empty() || id.find(',') != string::npos) {
            cout << "Customer ID cannot be empty or contain commas." << endl;
            return;
        }
        if (customers.find(id)) {
            cout << "Customer ID already exists." << endl;
            return;
        }
        cout << "Enter Customer Name: ";
        getline(cin, name);
        cout << "Enter Customer Email: ";
        getline(cin, email);
        if (name.empty() || email.empty()) {
            cout << "All fields are required." << endl;
            return;
        }
        try {
            stoi(id);
            customers.insert(Customer(id, name, email));
            saveCustomers();
            cout << "Customer added successfully." << endl;
        } catch (...) {
            cout << "Customer ID must be a valid number." << endl;
        }
    }

    void listCustomers() const {
        if (customers.isEmpty()) {
            cout << "No customers available." << endl;
            return;
        }
        cout << "\n--- Customer List ---" << endl;
        auto allCustomers = customers.getAllCustomers();
        for (const auto& c : allCustomers) {
            cout << c.toString() << endl;
        }
    }

    void viewMonthlySales() {
        cout << "\n--- Monthly Sales ---" << endl;
        string monthYear;
        cout << "Enter month and year (e.g., 06-2025): ";
        cin.ignore();
        getline(cin, monthYear);
        if (monthYear.length() != 7 || monthYear[2] != '-') {
            cout << "Invalid format. Use MM-YYYY." << endl;
            return;
        }
        double sales = monthlySales.get(monthYear);
        cout << "Sales for " << monthYear << ": $" << sales << endl;
        cout << "Update sales from orders? (yes/no): ";
        string update;
        getline(cin, update);
        transform(update.begin(), update.end(), update.begin(), ::tolower);
        if (update == "yes") {
            priority_queue<Order, vector<Order>, OrderComparator> temp = orders;
            while (!temp.empty()) {
                const Order& order = temp.top();
                string orderMonthYear = order.timestamp.substr(5, 5);
                monthlySales.insert(orderMonthYear, order.totalPrice);
                temp.pop();
            }
            saveSales();
            sales = monthlySales.get(monthYear);
            cout << "Updated sales for " << monthYear << ": $" << sales << endl;
        }
    }

    void trackShipments() const {
        if (!fs::exists(SHIPMENTS_FILE)) {
            cout << "Shipments.txt file not found." << endl;
            return;
        }
        ifstream ifs(SHIPMENTS_FILE);
        if (!ifs.is_open()) {
            cout << "Cannot open Shipments.txt. Please check file permissions." << endl;
            return;
        }
        int deliveredCount = 0, inProgressCount = 0;
        string line;
        while (getline(ifs, line)) {
            size_t pos = line.rfind(",");
            if (pos != string::npos) {
                string status = line.substr(pos + 1);
                transform(status.begin(), status.end(), status.begin(), ::tolower);
                if (status == "delivered")
                    deliveredCount++;
                else if (status == "in progress")
                    inProgressCount++;
            }
        }
        ifs.close();
        cout << "\n--- Shipment Status ---" << endl;
        cout << "Delivered Orders: " << deliveredCount
             << "\nIn-Progress Orders: " << inProgressCount << endl;
    }

    void customerMenu() {
        while (true) {
            cout << "\n--- FAMIN E-Commerce Customer Menu ---" << endl;
            cout << "1. View All Products\n2. View Men Products\n3. View Women Products\n"
                 << "4. Add to Cart\n5. View Cart\n6. Place Order\n0. Back to Main Menu\nChoice: ";
            int choice;
            if (!(cin >> choice)) {
                cout << "Invalid input. Enter a number." << endl;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                continue;
            }
            cin.ignore();
            if (choice == 0)
                break;
            switch (choice) {
            case 1:
                displayProducts();
                break;
            case 2:
                filterAndDisplayProducts("Men");
                break;
            case 3:
                filterAndDisplayProducts("Women");
                break;
            case 4: {
                string id;
                int qty;
                cout << "Enter Product ID: ";
                getline(cin, id);
                cout << "Enter Quantity: ";
                if (!(cin >> qty)) {
                    cout << "Invalid quantity." << endl;
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    continue;
                }
                cin.ignore();
                addToCart(id, qty);
                break;
            }
            case 5:
                cart.displayCart();
                break;
            case 6:
                placeOrder();
                break;
            default:
                cout << "Invalid choice." << endl;
            }
        }
    }

    bool authenticateAdmin() {
        string username, password;
        cout << "\n--- Admin Login ---" << endl;
        cout << "Username: ";
        cin.ignore();
        getline(cin, username);
        cout << "Password: ";
        getline(cin, password);

        if (adminTable.authenticate(username, password)) {
            cout << "Login successful!" << endl;
            return true;
        } else {
            cout << "Invalid username or password!" << endl;
            return false;
        }
    }

    void adminMenu() {
        while (true) {
            cout << "\n--- FAMIN Admin Control Panel ---" << endl;
            cout << "1. List Products\n2. Add Product\n3. Edit Product\n4. Delete Product\n"
                 << "5. Find Customer\n6. Remove Customer\n7. List Orders\n8. View Monthly Sales\n"
                 << "9. Track Shipments\n10. Add New Admin\n0. Back to Main Menu\nChoice: ";
            int choice;
            if (!(cin >> choice)) {
                cout << "Invalid input. Enter a number." << endl;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                continue;
            }
            cin.ignore();
            if (choice == 0)
                break;
            switch (choice) {
            case 1:
                listProducts();
                break;
            case 2:
                addProduct();
                break;
            case 3:
                editProduct();
                break;
            case 4:
                deleteProduct();
                break;
            case 5:
                findCustomerMenu();
                break;
            case 6:
                removeCustomer();
                break;
            case 7:
                listOrders();
                break;
            case 8:
                viewMonthlySales();
                break;
            case 9:
                trackShipments();
                break;
            case 10:
                addNewAdmin();
                break;
            default:
                cout << "Invalid choice." << endl;
            }
        }
    }

    void addNewAdmin() {
        string username, password, confirmPassword;
        cout << "\n--- Add New Admin ---" << endl;
        cout << "Username: ";
        getline(cin, username);
        cout << "Password: ";
        getline(cin, password);
        cout << "Confirm Password: ";
        getline(cin, confirmPassword);

        adminTable.addAdmin(username, password, confirmPassword);
    }

public:
    FaminEcommerce() {
        if (!ensureDirectoriesExist()) {
            cerr << "Fatal error: Cannot initialize directories. Exiting..." << endl;
            exit(1);
        }
        loadIdCounters();
        loadProducts();
        loadOrders();
        loadCustomers();
        loadSales();
    }

    void run() {
        while (true) {
            cout << "\n--- FAMIN E-Commerce System ---" << endl;
            cout << "1. Customer Mode\n2. Admin Mode\n0. Exit\nChoice: ";
            int choice;
            if (!(cin >> choice)) {
                cout << "Invalid input. Enter a number." << endl;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                continue;
            }
            if (choice == 0) {
                cout << "Exiting..." << endl;
                break;
            }
            switch (choice) {
            case 1:
                customerMenu();
                break;
            case 2:
                if (authenticateAdmin()) {
                    adminMenu();
                }
                break;
            default:
                cout << "Invalid choice." << endl;
            }
        }
    }
};

// Initialize static members
unsigned long FaminEcommerce::nextOrderId = 1;
unsigned long FaminEcommerce::nextTrackingId = 1;

int main() {
    srand(time(nullptr));
    FaminEcommerce ecommerce;
    ecommerce.run();
    return 0;
}
