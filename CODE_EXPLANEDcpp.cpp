#ifndef GLOBALUP_H     // Include guard start
#define GLOBALUP_H

// ... libraries listed above ...
using namespace std;   // Allows writing string instead of std::string everywhere

// ── extern declarations ────────────────────────────────────────────────────
extern Post postArray[];
extern int postCount;
// 'extern' means: "these variables exist in ANOTHER file (main.cpp)"
// "I want to USE them here but I'm not DEFINING them."
// Without extern, each file that includes globalUp.h would create its own
// separate postArray — they'd be different arrays and changes wouldn't sync.
// With extern, ALL files share the SAME single postArray from main.cpp.


// ════════════════════════════════════════════════════════════════════════════
// FUNCTION: hashPassword()
// PURPOSE:  Convert a plain-text password into a large number (the hash).
//           Passwords are NEVER stored as plain text — only the hash is saved.
// ════════════════════════════════════════════════════════════════════════════
unsigned long hashPassword(const string& password) {
    // 'unsigned long' = a large positive integer (no negative values)
    //                   On most systems this is 4-8 bytes, holding huge numbers
    // 'const string&' = take a reference (no copy), and promise not to modify it

    unsigned long pass = 5381;
    // Start with a specific seed value (5381 is the standard djb2 seed)
    // This number was chosen empirically — it spreads hash values well

    for (char c : password) {
        // Range-based for loop: iterate over EVERY CHARACTER in the password
        // 'c' holds each character one at a time (like 'p', 'a', 's', 's')

        pass = ((pass << 5) + pass) + c;
        // STEP BY STEP:
        //   pass << 5    = left-shift pass by 5 bits = multiply by 2^5 = multiply by 32
        //   (pass<<5) + pass = 32*pass + pass = 33*pass   (multiply by 33)
        //   + c          = add the ASCII value of this character
        //
        // Example for password "ab":
        //   Start: pass = 5381
        //   char 'a' (ASCII 97):  pass = 33*5381 + 97  = 177570
        //   char 'b' (ASCII 98):  pass = 33*177570 + 98 = 5859908
        //
        // Same password ALWAYS gives same number. Different passwords give
        // different numbers (with extremely rare collisions).
    }
    return pass;
    // Returns the final hash number
}


// ════════════════════════════════════════════════════════════════════════════
// FUNCTION: getPasswordMasked()
// PURPOSE:  Read a password from keyboard, showing '*' instead of actual chars
//           Works on both Windows and Linux/Mac using conditional compilation
// ════════════════════════════════════════════════════════════════════════════
string getPasswordMasked() {
    string password = "";   // Start with empty string — we'll build it char by char
    char ch;                // Holds the current keypress

#ifdef _WIN32
    // ── WINDOWS IMPLEMENTATION ───────────────────────────────────────────
    while ((ch = _getch()) != '\r') {
        // _getch() = reads ONE character without displaying it on screen
        //            and without waiting for Enter (non-buffered input)
        // '\r' = carriage return = the Enter key on Windows
        // Loop runs until Enter is pressed

        if (ch == '\b') {
            // '\b' = backspace key was pressed
            if (!password.empty()) {
                cout << "\b \b";
                // "\b \b" trick:
                //   \b = move cursor left one position
                //   (space) = overwrite the '*' character with a blank
                //   \b = move cursor left again (ready to overwrite)
                // Net effect: the '*' is visually erased from screen
                password.pop_back();
                // pop_back() removes the LAST character from the string
                // Matches what was visually erased
            }
        } else {
            password += ch;      // Append the typed character to our string
            cout << '*';         // Show '*' on screen instead of the actual char
        }
    }

#else
    // ── LINUX/MAC IMPLEMENTATION ──────────────────────────────────────────
    termios oldt, newt;
    // 'termios' is a struct that holds terminal settings
    // 'oldt' saves the ORIGINAL settings so we can restore them later
    // 'newt' will be the MODIFIED settings

    tcgetattr(STDIN_FILENO, &oldt);
    // tcgetattr = "terminal control get attributes"
    // STDIN_FILENO = file descriptor 0 = standard input (keyboard)
    // &oldt = pointer to where to store the current settings
    // This reads the current terminal configuration into 'oldt'

    newt = oldt;
    // Copy all settings from old to new — we'll only change what we need

    newt.c_lflag &= ~(ECHO);
    // c_lflag = "local flags" — controls local terminal behavior
    // ECHO    = the flag that makes typed characters appear on screen
    // ~(ECHO) = bitwise NOT of ECHO = all bits set EXCEPT the ECHO bit
    // &=      = bitwise AND-assign
    // Combined: turns OFF the ECHO bit, leaving all other flags unchanged
    // Result: characters typed will NOT appear on screen

    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    // tcsetattr = "terminal control set attributes"
    // TCSANOW = "Terminal Control Set Attributes NOW" = apply immediately
    // &newt   = pointer to our modified settings
    // This applies our new no-echo settings to the terminal

    while ((ch = getchar()) != '\n') {
        // getchar() = reads one character from standard input
        // '\n' = newline = Enter key on Linux/Mac
        if (ch == 127 || ch == '\b') {
            // On Linux, backspace often sends ASCII 127 (DEL) instead of '\b'
            // We handle both cases
            if (!password.empty()) {
                cout << "\b \b";       // Erase the '*' visually
                password.pop_back();   // Remove last char from string
            }
        } else {
            password += ch;   // Build the password string
            cout << '*';      // Show '*' instead of the real character
        }
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    // CRITICAL: Restore the original terminal settings!
    // If we don't do this, the terminal stays in no-echo mode AFTER
    // the program exits — the user wouldn't see what they type in the terminal!
#endif

    cout << "\n";   // Print a newline after Enter is pressed (since Enter itself wasn't echoed)
    return password;
}


// ════════════════════════════════════════════════════════════════════════════
// ENUM: Role
// PURPOSE: Represent the three types of users with readable names
// ════════════════════════════════════════════════════════════════════════════
enum Role { GUEST, USER, ADMIN };
// 'enum' = enumeration = a named set of integer constants
// The compiler assigns: GUEST=0, USER=1, ADMIN=2
// WHY use enum instead of int?
//   Bad:  if (role == 2) ...        ← what does 2 mean? unclear
//   Good: if (role == ADMIN) ...    ← immediately readable


// ── roleToString() ────────────────────────────────────────────────────────
string roleToString(Role r) {
    if (r == ADMIN) return "ADMIN";
    if (r == USER)  return "USER";
    return "GUEST";
    // Converts the enum value to text for file storage
    // Files store "ADMIN" not 2 — more readable and less fragile
}


// ── stringToRole() ────────────────────────────────────────────────────────
Role stringToRole(const string& s) {
    if (s == "ADMIN") return ADMIN;
    if (s == "USER")  return USER;
    return GUEST;
    // Converts text back to enum when loading from file
    // If the string is anything unexpected, defaults to GUEST (safe fallback)
}


// ════════════════════════════════════════════════════════════════════════════
// STRUCT: Credential
// PURPOSE: A simple data container for one user account's information
// ════════════════════════════════════════════════════════════════════════════
struct Credential {
    string        username;      // The user's login name
    string        email;         // The user's email address
    unsigned long passwordHash;  // The HASHED password (not the real password)
    Role          role;          // GUEST, USER, or ADMIN
};
// 'struct' vs 'class': struct has all members public by default.
// It's used here as a plain data holder — no methods, just data grouped together.
// Each user account is one Credential object in memory.


// ── Global User Database ───────────────────────────────────────────────────
vector<Credential> credentialStore;
// This is the IN-MEMORY database of all user accounts.
// Loaded from users.txt at startup.
// All registrations, deletions, and promotions modify this vector.
// Saved back to users.txt on every change and at shutdown.
// vector = dynamic array that can grow and shrink automatically.

const string USERS_FILE = "users.txt";
// Constant for the filename. Using a constant means if you ever rename the
// file, you only change it in ONE place, not scattered throughout the code.


// ════════════════════════════════════════════════════════════════════════════
// FUNCTION: saveAllToFile()
// PURPOSE:  Write the entire credentialStore vector to users.txt
//           Called after every change to keep file in sync with memory
// ════════════════════════════════════════════════════════════════════════════
void saveAllToFile() {
    ofstream file(USERS_FILE);
    // Opens users.txt for writing. OVERWRITES the file completely.
    // This is intentional — we always write the full current state.

    if (!file) {
        cout << "[Error] Could not open users.txt for writing.\n";
        return;
        // 'if (!file)' is shorthand for 'if (!file.is_open())'
        // ofstream overloads operator! to return true when file failed to open
    }

    for (const Credential& c : credentialStore) {
        // 'const Credential& c' = read-only reference to each element
        // No copy is made, and we can't accidentally modify elements
        file << c.username         << "|"
             << c.email            << "|"
             << c.passwordHash     << "|"
             << roleToString(c.role) << "\n";
        // Each user = one line: nidal|nidal@email.com|193485737|ADMIN
    }
    file.close();
}


// ════════════════════════════════════════════════════════════════════════════
// FUNCTION: loadFromFile()
// PURPOSE:  Read users.txt and fill credentialStore vector
//           Called at startup (in addition to FileHandler::loadUsers)
// ════════════════════════════════════════════════════════════════════════════
void loadFromFile() {
    ifstream file(USERS_FILE);
    if (!file) return;   // File doesn't exist yet — first run, no users saved

    credentialStore.clear();
    // Clear existing data before loading — prevents duplicates

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;   // Skip blank lines

        stringstream ss(line);
        // Wrap line in a stream. For "nidal|nidal@email.com|193485737|ADMIN"
        // ss now acts like a stream containing that text

        string uname, email, hashStr, roleStr;
        getline(ss, uname,   '|');   // Reads "nidal"
        getline(ss, email,   '|');   // Reads "nidal@email.com"
        getline(ss, hashStr, '|');   // Reads "193485737"
        getline(ss, roleStr, '|');   // Reads "ADMIN"
        // Each getline call reads UP TO the '|' delimiter

        Credential c;
        c.username     = uname;
        c.email        = email;
        c.passwordHash = stoul(hashStr);
        // stoul = "string to unsigned long"
        // Converts the text "193485737" back into the number 193485737
        c.role         = stringToRole(roleStr);   // "ADMIN" → ADMIN

        credentialStore.push_back(c);
        // Append this credential to the end of the vector
    }
    file.close();
}


// ── usernameExists() ──────────────────────────────────────────────────────
bool usernameExists(const string& username) {
    for (const Credential& c : credentialStore)
        if (c.username == username) return true;
    return false;
    // Linear search through all stored credentials.
    // Returns true immediately when a match is found (short-circuit).
    // If loop completes without finding one, returns false.
    // Used during registration to prevent duplicate usernames.
}


// ── emailExists() ─────────────────────────────────────────────────────────
bool emailExists(const string& email) {
    for (const Credential& c : credentialStore)
        if (c.email == email) return true;
    return false;
    // Same pattern as usernameExists but checks email field.
    // Prevents two accounts with the same email address.
}


// ════════════════════════════════════════════════════════════════════════════
// FUNCTION: findCredential()
// PURPOSE:  Verify login credentials. Returns true if valid, sets outRole.
// ════════════════════════════════════════════════════════════════════════════
bool findCredential(const string& username,
                    const string& email,
                    const string& password,
                    Role&         outRole) {
    // Note: 'password' comes in as plain text; we hash it first
    // 'Role& outRole' = reference parameter used as OUTPUT
    //   After this function runs, the caller's role variable will be set
    //   This is how we return TWO things (bool success + Role) from one function

    unsigned long inputHash = hashPassword(password);
    // Convert the typed password to a hash for comparison
    // We NEVER compare plain text passwords — only hashes

    for (const Credential& c : credentialStore) {
        if (c.username     == username &&   // All three must match
            c.email        == email    &&
            c.passwordHash == inputHash) {  // Hash comparison — secure!
            outRole = c.role;               // Set the output parameter
            return true;                    // Login successful
        }
    }
    return false;   // No matching credential found
}


// ════════════════════════════════════════════════════════════════════════════
// FUNCTION: addCredential()
// PURPOSE:  Create a new user account and save immediately to file
// ════════════════════════════════════════════════════════════════════════════
void addCredential(const string& username,
                   const string& email,
                   const string& password,
                   Role          role) {
    Credential c;
    c.username     = username;
    c.email        = email;
    c.passwordHash = hashPassword(password);   // HASH the password before storing
    c.role         = role;
    credentialStore.push_back(c);   // Add to in-memory database
    saveAllToFile();                // Immediately persist to disk
    // WHY save immediately? So if the program crashes right after
    // registration, the new account is not lost.
}


// ════════════════════════════════════════════════════════════════════════════
// CLASS: Permission
// PURPOSE: Centralized access control — defines who can do what
// ════════════════════════════════════════════════════════════════════════════
class Permission {
public:
    // ALL methods are 'static':
    // - Called as Permission::canpost(role)  NOT  Permission p; p.canpost(role)
    // - No object needed — these are utility functions
    // - All take a Role and return bool

    static bool canviewpost          (Role r) { return true; }
    // EVERYONE can view posts (Guest, User, Admin)
    // 'return true' = no role check needed

    static bool cansearch            (Role r) { return true; }
    // EVERYONE can search

    static bool canpost              (Role r) { return r == USER || r == ADMIN; }
    // Only registered users and admins can post.
    // Guests (r == GUEST) get false — blocked.
    // '||' = logical OR — true if EITHER condition is true

    static bool canLike              (Role r) { return r == USER || r == ADMIN; }
    static bool cancomment           (Role r) { return r == USER || r == ADMIN; }
    static bool canSendFriendRequest (Role r) { return r == USER || r == ADMIN; }
    // Same rule for liking, commenting, friend requests

    static bool canDeletePost        (Role r) { return r == ADMIN; }
    static bool canDeleteAccount     (Role r) { return r == ADMIN; }
    static bool canDeleteUser        (Role r) { return r == ADMIN; }
    static bool canPromote           (Role r) { return r == ADMIN; }
    static bool canViewStats         (Role r) { return r == ADMIN; }
    static bool canCreateAdmin       (Role r) { return r == ADMIN; }
    // ADMIN-only operations — only r == ADMIN gives true
};


// ════════════════════════════════════════════════════════════════════════════
// CLASS: User  (BASE CLASS)
// PURPOSE: Represents any logged-in user. Contains ALL action methods.
//          Guest, regularUser, Admin all INHERIT from this class.
// ════════════════════════════════════════════════════════════════════════════
class User {
protected:
    // 'protected' = private to outsiders, but ACCESSIBLE to child classes
    // If these were 'private', Guest/regularUser/Admin couldn't access name/role
    string name;   // The username of the logged-in user
    Role   role;   // Their role: GUEST, USER, or ADMIN

public:
    User(string n, Role r) : name(n), role(r) {}
    // Constructor with INITIALIZER LIST ': name(n), role(r)'
    // This directly initializes members before the constructor body runs.
    // Equivalent to but more efficient than: User(string n, Role r) { name=n; role=r; }

    // ── viewpost() ────────────────────────────────────────────────────────
    void viewpost() {
        if (!Permission::canviewpost(role)) {
            // The ! (NOT) operator — if canviewpost returns false, enter block
            cout << name << " is not allowed to view posts\n";
            return;   // Exit the function early — don't proceed
        }
        if (postCount == 0) {
            cout << "No posts available.\n";
            return;   // Nothing to display
        }
        cout << "\n--- All Posts ---\n";
        for (int i = 0; i < postCount; i++) {
            cout << "[Post " << i << "]\n";
            postArray[i].displayPost();   // Call Post's display method
        }
        // postCount is the global variable from main.cpp (accessed via extern)
    }

    // ── search() ──────────────────────────────────────────────────────────
    void search() {
        if (!Permission::cansearch(role)) {
            cout << name << " is not allowed to search\n";
            return;
        }
        if (postCount == 0) {
            cout << "No posts available.\n";
            return;
        }

        cin.ignore();
        // IMPORTANT: cin >> choice (used in userMenu) leaves '\n' in the buffer.
        // getline would immediately read that '\n' as empty input.
        // cin.ignore() discards ONE character (the '\n') from the buffer.
        // Without this, search box appears to be skipped.

        string searchUsername;
        cout << "Enter username to search: ";
        getline(cin, searchUsername);
        // getline reads everything including spaces until Enter

        bool found = false;
        cout << "\n--- Posts by " << searchUsername << " ---\n";
        for (int i = 0; i < postCount; i++) {
            if (postArray[i].getUsername() == searchUsername) {
                cout << "[Post " << i << "]\n";
                postArray[i].displayPost();
                found = true;
            }
        }
        if (!found) {
            cout << "No posts found by user: " << searchUsername << "\n";
        }
    }

    // ── createPost() ──────────────────────────────────────────────────────
    void createPost() {
        if (!Permission::canpost(role)) {
            cout << name << " is not allowed to create a post\n";
            return;
        }
        if (postCount >= 100) {
            // The global postArray has 100 slots — can't exceed it
            cout << "Post limit reached. Cannot create more posts.\n";
            return;
        }

        cin.ignore();   // Flush the '\n' left by cin >> choice

        string caption;
        cout << "Enter post caption: ";
        getline(cin, caption);   // Read full caption including spaces

        postArray[postCount] = Post(name, caption);
        // Create a new Post object and store it at the next free slot.
        // postCount is the index of the next empty slot.
        postCount++;
        // Move the counter forward — next post goes in the next slot.

        cout << "Post created successfully by " << name << "\n";
    }

    // ── likePost() ────────────────────────────────────────────────────────
    void likePost() {
        if (!Permission::canLike(role)) {
            cout << name << " is not allowed to like posts\n";
            return;
        }
        if (postCount == 0) {
            cout << "No posts available to like.\n";
            return;
        }

        // Display all posts with their index numbers
        cout << "\n--- Posts ---\n";
        for (int i = 0; i < postCount; i++) {
            cout << "[Post " << i << "]\n";
            postArray[i].displayPost();
        }

        int index;
        cout << "Enter post index to like: ";
        cin >> index;
        // cin >> reads an integer and leaves '\n' in buffer
        // Note: next operations in this function don't need getline, so
        // cin.ignore() is NOT needed here

        if (index >= 0 && index < postCount) {
            postArray[index].likePost();
            // Calls Post::likePost() which does likes++
            cout << name << " liked post " << index << "\n";
        } else {
            cout << "Invalid post index.\n";
        }
    }

    // ── commentPost() ─────────────────────────────────────────────────────
    void commentPost() {
        if (!Permission::cancomment(role)) {
            cout << name << " is not allowed to comment\n";
            return;
        }
        if (postCount == 0) {
            cout << "No posts available to comment on.\n";
            return;
        }

        // Show all posts
        cout << "\n--- Posts ---\n";
        for (int i = 0; i < postCount; i++) {
            cout << "[Post " << i << "]\n";
            postArray[i].displayPost();
        }

        int index;
        cout << "Enter post index to comment on: ";
        cin >> index;

        if (index >= 0 && index < postCount) {
            cin.ignore();
            // Here we DO need cin.ignore() because after cin >> index
            // we then call getline — so the '\n' must be flushed.

            string commentText;
            cout << "Enter comment: ";
            getline(cin, commentText);

            postArray[index].addComment(name, commentText);
            // Calls Post::addComment(username, text)
            // The Post stores it in its comment array
            cout << "Comment added successfully.\n";
        } else {
            cout << "Invalid post index.\n";
        }
    }

    // ── sendFriendRequest() ───────────────────────────────────────────────
    void sendFriendRequest() {
        if (!Permission::canSendFriendRequest(role)) {
            cout << name << " is not allowed to send friend requests\n";
            return;
        }

        cin.ignore();
        string friendUsername;
        cout << "Enter username to send friend request: ";
        getline(cin, friendUsername);

        // Check if the target user exists in credentialStore
        bool found = false;
        for (int i = 0; i < credentialStore.size(); i++) {
            if (credentialStore[i].username == friendUsername) {
                found = true;
                break;   // Stop searching once found
            }
        }

        if (found) {
            cout << name << " sent a friend request to " << friendUsername << "\n";
            // Note: actual friend relationship storage is not yet implemented.
            // This demonstrates the feature logic/UI exists but persistence is future work.
        } else {
            cout << "User not found.\n";
        }
    }

    // ── deletePost() ──────────────────────────────────────────────────────
    void deletePost() {
        if (!Permission::canDeletePost(role)) {
            // ONLY ADMIN — regular users cannot delete posts
            cout << name << " is not allowed to delete posts\n";
            return;
        }
        if (postCount == 0) {
            cout << "No posts available to delete.\n";
            return;
        }

        cout << "\n--- Posts ---\n";
        for (int i = 0; i < postCount; i++) {
            cout << "[Post " << i << "]\n";
            postArray[i].displayPost();
        }

        int index;
        cout << "Enter post index to delete: ";
        cin >> index;

        if (index >= 0 && index < postCount) {
            // SHIFT all posts after the deleted one LEFT by one position
            for (int i = index; i < postCount - 1; i++) {
                postArray[i] = postArray[i + 1];
                // Copy the next element into the current position
                // This overwrites the deleted post and closes the gap
            }
            postCount--;
            // The last slot is now stale (a copy of the second-to-last post)
            // but postCount-- ensures it's never accessed again.
            cout << "Post deleted successfully.\n";
        } else {
            cout << "Invalid post index.\n";
        }
    }

    // ── DeleteAccount() ───────────────────────────────────────────────────
    void DeleteAccount() {
        if (!Permission::canDeleteAccount(role)) {
            cout << name << " is not allowed to delete accounts\n";
            return;
        }

        cin.ignore();
        string userToDelete;
        cout << "Enter username to delete: ";
        getline(cin, userToDelete);

        for (int i = 0; i < credentialStore.size(); i++) {
            if (credentialStore[i].username == userToDelete) {
                credentialStore.erase(credentialStore.begin() + i);
                // credentialStore.begin() = iterator pointing to element [0]
                // + i = advance iterator to position i
                // erase() removes that element and shifts everything after it left
                // The vector automatically shrinks in size by 1

                saveAllToFile();   // Persist the deletion immediately
                cout << "Account '" << userToDelete << "' deleted successfully.\n";
                return;   // Done — exit function
            }
        }
        cout << "User not found.\n";
    }

    // ── DeleteUser() ──────────────────────────────────────────────────────
    void DeleteUser() {
        // Identical logic to DeleteAccount() — both remove a user from credentialStore.
        // They exist as separate methods to demonstrate distinct admin capabilities
        // in the menu (conceptually: "delete account" vs "delete user").
        if (!Permission::canDeleteUser(role)) {
            cout << name << " is not allowed to delete users\n";
            return;
        }
        cin.ignore();
        string userToDelete;
        cout << "Enter username to delete: ";
        getline(cin, userToDelete);

        for (int i = 0; i < credentialStore.size(); i++) {
            if (credentialStore[i].username == userToDelete) {
                credentialStore.erase(credentialStore.begin() + i);
                saveAllToFile();
                cout << "User '" << userToDelete << "' deleted successfully.\n";
                return;
            }
        }
        cout << "User not found.\n";
    }

    // ── promoteUser() ─────────────────────────────────────────────────────
    void promoteUser() {
        if (!Permission::canPromote(role)) {
            cout << name << " is not allowed to promote users\n";
            return;
        }
        cin.ignore();
        string userToPromote;
        cout << "Enter username to promote to Admin: ";
        getline(cin, userToPromote);

        for (int i = 0; i < credentialStore.size(); i++) {
            if (credentialStore[i].username == userToPromote) {
                if (credentialStore[i].role == ADMIN) {
                    cout << "User is already an Admin.\n";
                    return;   // Can't promote someone who's already admin
                }
                credentialStore[i].role = ADMIN;
                // DIRECTLY modify the role field in the stored Credential.
                // Since credentialStore holds Credential objects (not pointers),
                // this modifies the actual stored data in place.
                saveAllToFile();   // Persist the role change
                cout << "User '" << userToPromote << "' promoted to Admin.\n";
                return;
            }
        }
        cout << "User not found.\n";
    }

    // ── ViewStats() ───────────────────────────────────────────────────────
    void ViewStats() {
        if (!Permission::canViewStats(role)) {
            cout << name << " is not allowed to view stats\n";
            return;
        }
        cout << "\n--- System Statistics ---\n";
        cout << "Total Users: " << credentialStore.size() << "\n";
        // .size() returns the number of elements currently in the vector
        cout << "Total Posts: " << postCount << "\n";
    }

    // ── createAdminAccount() ──────────────────────────────────────────────
    void createAdminAccount() {
        if (!Permission::canCreateAdmin(role)) {
            cout << name << " is not allowed to create admin accounts\n";
            return;
        }

        string newUsername, newEmail, newPassword, confirmPassword;
        cin.ignore();   // Flush '\n' before using getline

        cout << "\n--- Create New Admin Account ---\n";

        cout << "New admin username : ";
        getline(cin, newUsername);
        if (usernameExists(newUsername)) { cout << "Username already taken.\n"; return; }

        cout << "New admin email    : ";
        getline(cin, newEmail);
        if (emailExists(newEmail)) { cout << "Email already registered.\n"; return; }

        cout << "New admin password : ";
        newPassword = getPasswordMasked();   // Masked input — shows '*'

        cout << "Confirm password   : ";
        confirmPassword = getPasswordMasked();

        if (newPassword != confirmPassword) { cout << "Passwords do not match.\n"; return; }
        if (newPassword.length() < 6)       { cout << "Password must be at least 6 characters.\n"; return; }
        // .length() returns the number of characters in the string

        addCredential(newUsername, newEmail, newPassword, ADMIN);
        // Pass ADMIN as the role — this account gets full admin privileges
        cout << "Admin account '" << newUsername << "' created and saved successfully.\n";
    }
};


// ════════════════════════════════════════════════════════════════════════════
// DERIVED CLASSES: Guest, regularUser, Admin
// PURPOSE: Three types of users, each inheriting everything from User
// ════════════════════════════════════════════════════════════════════════════
class Guest : public User {
public:
    Guest() : User("Guest", GUEST) {}
    // 'public User' = inherits from User using public inheritance
    // All of User's public methods become public in Guest too
    // The constructor calls User's constructor with "Guest" and GUEST
    // Guest has no name (always "Guest") and the lowest permission level
};

class regularUser : public User {
public:
    regularUser(string n) : User(n, USER) {}
    // Takes a name parameter — the logged-in user's actual username
    // Sets role to USER
};

class Admin : public User {
public:
    Admin(string n) : User(n, ADMIN) {}
    // Same as regularUser but sets role to ADMIN
    // Gets all the extra permissions (delete, promote, createAdmin, etc.)
};

// WHY inheritance here?
// All three classes have IDENTICAL method implementations (viewpost, createPost, etc.)
// Inheritance means we write those methods ONCE in User and all three get them.
// The only difference is the 'role' value, which Permission checks to allow/deny.


// ════════════════════════════════════════════════════════════════════════════
// FUNCTION: userMenu()
// PURPOSE:  The main interaction loop for any logged-in user or guest
// ════════════════════════════════════════════════════════════════════════════
void userMenu(User& u, Role role) {
    // 'User& u' = reference to a User object (or any subclass — polymorphism)
    // WHY reference? If we took User by value (User u), it would COPY the object
    // as a base User — losing the Admin/regularUser type. Reference preserves
    // the actual runtime type.
    // 'Role role' = passed separately to show/hide admin menu option

    int choice;
    do {
        // do-while guarantees the menu shows AT LEAST ONCE before checking condition
        cout << "\n----- MENU -----\n"
             << "1.  View Posts\n"
             << "2.  Search User\n"
             << "3.  Create Post\n"
             << "4.  Like Post\n"
             << "5.  Comment\n"
             << "6.  Send Friend Request\n"
             << "7.  Delete Post\n"
             << "8.  Delete Account\n"
             << "9.  Delete User\n"
             << "10. Promote User\n"
             << "11. View Stats\n";
        if (role == ADMIN)
             cout << "12. Create Admin Account\n";
        // Option 12 only APPEARS for admin — regular users don't even see it
        cout << "0.  Logout\n"
             << "Enter choice: ";
        cin >> choice;

        switch (choice) {
            case 1:  u.viewpost();           break;
            case 2:  u.search();             break;
            case 3:  u.createPost();         break;
            case 4:  u.likePost();           break;
            case 5:  u.commentPost();        break;
            case 6:  u.sendFriendRequest();  break;
            case 7:  u.deletePost();         break;
            case 8:  u.DeleteAccount();      break;
            case 9:  u.DeleteUser();         break;
            case 10: u.promoteUser();        break;
            case 11: u.ViewStats();          break;
            case 12:
                if (role == ADMIN) u.createAdminAccount();
                else cout << "Invalid choice\n";
                // Double-check even though option 12 only shows for admins
                break;
            case 0:  cout << "Logging out...\n"; break;
            default: cout << "Invalid choice\n";
        }
    } while (choice != 0);
    // Loop continues until user enters 0 (logout)
}


// ════════════════════════════════════════════════════════════════════════════
// FUNCTION: registerUser()
// PURPOSE:  Let a new user create an account (always as USER role)
// ════════════════════════════════════════════════════════════════════════════
void registerUser() {
    string username, email, password, confirmPassword;
    cin.ignore();   // Flush '\n' left by cin >> choice in main menu

    cout << "\n--- New User Registration ---\n";

    // ── Username validation ────────────────────────────────────────────────
    cout << "Choose a username  : ";
    getline(cin, username);
    if (username.empty())         { cout << "Username cannot be empty.\n";   return; }
    if (usernameExists(username)) { cout << "Username already taken.\n";     return; }
    // 'return' exits the function immediately — called "guard clauses" or "early returns"
    // Avoids deeply nested if-else chains

    // ── Email validation ───────────────────────────────────────────────────
    cout << "Enter your email   : ";
    getline(cin, email);
    if (email.empty())            { cout << "Email cannot be empty.\n";      return; }
    if (emailExists(email))       { cout << "Email already registered.\n";   return; }

    // ── Password validation ────────────────────────────────────────────────
    cout << "Choose a password  : ";
    password = getPasswordMasked();   // Show '*' while typing
    if (password.length() < 6)   { cout << "Password must be at least 6 characters.\n"; return; }

    cout << "Confirm password   : ";
    confirmPassword = getPasswordMasked();
    if (password != confirmPassword) { cout << "Passwords do not match.\n"; return; }

    // ── If all checks pass, create the account ─────────────────────────────
    addCredential(username, email, password, USER);
    // USER (not ADMIN) — self-registration never grants admin access
    cout << "Registration successful! You can now log in as '" << username << "'.\n";
}


// ════════════════════════════════════════════════════════════════════════════
// FUNCTION: loginAndEnter()
// PURPOSE:  Authenticate a user and enter the application
// ════════════════════════════════════════════════════════════════════════════
void loginAndEnter() {
    string username, email, password;
    cin.ignore();   // Flush '\n' before getline calls

    cout << "\n--- Login ---\n";
    cout << "Username : ";  getline(cin, username);
    cout << "Email    : ";  getline(cin, email);
    cout << "Password : ";  password = getPasswordMasked();

    Role role;   // Uninitialized — will be set by findCredential if login succeeds
    if (!findCredential(username, email, password, role)) {
        // findCredential hashes the password and checks all credentials
        // Returns false if no match found
        cout << "Invalid credentials. Access denied.\n";
        return;   // Exit — don't proceed into the app
    }

    cout << "Login successful! Welcome, " << username << ".\n";

    if (role == ADMIN) {
        Admin a(username);        // Create an Admin object with this username
        userMenu(a, ADMIN);       // Enter the menu with full admin permissions
    } else {
        regularUser u(username);  // Create a regularUser object
        userMenu(u, USER);        // Enter the menu with user permissions
    }
    // When userMenu returns (user logged out), these objects are destroyed.
    // Stack allocation — no memory leak possible.
}


// ════════════════════════════════════════════════════════════════════════════
// FUNCTION: firstTimeAdminSetup()
// PURPOSE:  Force creation of a super-admin when users.txt is empty
//           Called only when credentialStore.empty() at startup
// ════════════════════════════════════════════════════════════════════════════
void firstTimeAdminSetup() {
    cout << "-----------------------------------------\n";
    cout << "   FIRST TIME SETUP - Create Super Admin\n";
    cout << "-----------------------------------------\n";
    cout << "No users found. You must create the first Admin account.\n\n";

    string username, email, password, confirmPassword;
    cin.ignore();

    cout << "Admin username : ";
    getline(cin, username);
    if (username.empty()) {
        cout << "Username cannot be empty.\n";
        exit(1);
        // 'exit(1)' terminates the ENTIRE PROGRAM immediately with error code 1
        // WHY exit() and not return?
        // 'return' would go back to main(), but without an admin the system
        // is completely unusable. exit() hard-stops everything.
        // Exit code 1 tells the OS that the program ended abnormally.
    }

    cout << "Admin email    : ";
    getline(cin, email);
    if (email.empty()) { cout << "Email cannot be empty.\n"; exit(1); }

    cout << "Admin password : ";
    password = getPasswordMasked();
    if (password.length() < 6) { cout << "Password must be at least 6 characters.\n"; exit(1); }

    cout << "Confirm        : ";
    confirmPassword = getPasswordMasked();
    if (password != confirmPassword) { cout << "Passwords do not match.\n"; exit(1); }

    addCredential(username, email, password, ADMIN);
    // Create the very first account with ADMIN role
    // This person can then create more admins and regular users

    cout << "\nSuper Admin '" << username << "' created successfully!\n";
    cout << "You can now log in.\n";
    cout << "-----------------------------------------\n\n";
}

#endif   // GLOBALUP_H — end of include guard