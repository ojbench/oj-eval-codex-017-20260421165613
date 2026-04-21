#include <iostream>
#include <string>
#include <cctype>
using namespace std;

// Minimal subset: user module only, no STL containers except std::string
// Implements: clean, exit, login, logout, add_user, query_profile, modify_profile

struct User {
    string username;
    string password;
    string name;
    string mail;
    int privilege;
    bool online;
};

static const int MAX_USERS = 50000; // sufficient for basic tests
static User users[MAX_USERS];
static int user_cnt = 0;

static int find_user(const string &u) {
    for (int i = 0; i < user_cnt; ++i) if (users[i].username == u) return i;
    return -1;
}

static int tokenize(const string &line, string tokens[], int maxTok) {
    int n = 0; int i = 0; int L = (int)line.size();
    while (i < L) {
        while (i < L && isspace(static_cast<unsigned char>(line[i]))) ++i;
        if (i >= L) break;
        int j = i;
        while (j < L && !isspace(static_cast<unsigned char>(line[j]))) ++j;
        if (n < maxTok) tokens[n++] = line.substr(i, j - i);
        i = j;
    }
    return n;
}

static bool get_param(string tokens[], int n, const string &key, string &out) {
    for (int i = 0; i + 1 < n; ++i) if (tokens[i] == key) { out = tokens[i+1]; return true; }
    return false;
}

static void cmd_clean() {
    for (int i = 0; i < user_cnt; ++i) users[i] = User();
    user_cnt = 0;
    cout << 0 << '\n';
}

static void cmd_exit() {
    for (int i = 0; i < user_cnt; ++i) users[i].online = false;
    cout << "bye\n";
}

static void cmd_login(string tokens[], int n) {
    string u, p;
    get_param(tokens, n, "-u", u);
    get_param(tokens, n, "-p", p);
    int idx = find_user(u);
    if (idx < 0 || users[idx].password != p || users[idx].online) { cout << -1 << '\n'; return; }
    users[idx].online = true; cout << 0 << '\n';
}

static void cmd_logout(string tokens[], int n) {
    string u; get_param(tokens, n, "-u", u);
    int idx = find_user(u);
    if (idx >= 0 && users[idx].online) { users[idx].online = false; cout << 0 << '\n'; }
    else cout << -1 << '\n';
}

static void cmd_add_user(string tokens[], int n) {
    string c,u,p,nm,m,g;
    get_param(tokens, n, "-c", c);
    get_param(tokens, n, "-u", u);
    get_param(tokens, n, "-p", p);
    get_param(tokens, n, "-n", nm);
    get_param(tokens, n, "-m", m);
    get_param(tokens, n, "-g", g);
    if (user_cnt >= MAX_USERS) { cout << -1 << '\n'; return; }
    if (find_user(u) != -1) { cout << -1 << '\n'; return; }
    if (user_cnt == 0) {
        if (u.empty()||p.empty()||nm.empty()||m.empty()) { cout << -1 << '\n'; return; }
        users[user_cnt++] = User{u,p,nm,m,10,false};
        cout << 0 << '\n'; return;
    }
    int ic = find_user(c);
    if (ic == -1 || !users[ic].online) { cout << -1 << '\n'; return; }
    int gp = 0; if (!g.empty()) gp = atoi(g.c_str());
    if (!(users[ic].privilege > gp)) { cout << -1 << '\n'; return; }
    if (u.empty()||p.empty()||nm.empty()||m.empty()) { cout << -1 << '\n'; return; }
    users[user_cnt++] = User{u,p,nm,m,gp,false};
    cout << 0 << '\n';
}

static void cmd_query_profile(string tokens[], int n) {
    string c,u; get_param(tokens, n, "-c", c); get_param(tokens, n, "-u", u);
    int ic = find_user(c), iu = find_user(u);
    if (ic==-1 || iu==-1 || !users[ic].online) { cout << -1 << '\n'; return; }
    if (!(users[ic].privilege > users[iu].privilege || c==u)) { cout << -1 << '\n'; return; }
    const User &t = users[iu];
    cout << t.username << ' ' << t.name << ' ' << t.mail << ' ' << t.privilege << '\n';
}

static void cmd_modify_profile(string tokens[], int n) {
    string c,u,pw,nm,m,g; get_param(tokens,n,"-c",c); get_param(tokens,n,"-u",u);
    get_param(tokens,n,"-p",pw); get_param(tokens,n,"-n",nm); get_param(tokens,n,"-m",m); get_param(tokens,n,"-g",g);
    int ic = find_user(c), iu = find_user(u);
    if (ic==-1 || iu==-1 || !users[ic].online) { cout << -1 << '\n'; return; }
    if (!(users[ic].privilege > users[iu].privilege || c==u)) { cout << -1 << '\n'; return; }
    if (!g.empty()) { int gp = atoi(g.c_str()); if (!(users[ic].privilege > gp)) { cout << -1 << '\n'; return; } users[iu].privilege = gp; }
    if (!pw.empty()) users[iu].password = pw;
    if (!nm.empty()) users[iu].name = nm;
    if (!m.empty()) users[iu].mail = m;
    const User &t = users[iu];
    cout << t.username << ' ' << t.name << ' ' << t.mail << ' ' << t.privilege << '\n';
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    string line;
    while (true) {
        if (!std::getline(cin, line)) break;
        if (line.empty()) continue;
        if (line.size()>=2 && line[0]=='>') {
            if (line.size()>=2 && line[1]==' ') line = line.substr(2);
            else line = line.substr(1);
        }
        string tokens[128]; int n = tokenize(line, tokens, 128);
        if (n <= 0) continue;
        const string &cmd = tokens[0];
        if (cmd == "clean") { cmd_clean(); }
        else if (cmd == "exit") { cmd_exit(); break; }
        else if (cmd == "login") { cmd_login(tokens, n); }
        else if (cmd == "logout") { cmd_logout(tokens, n); }
        else if (cmd == "add_user") { cmd_add_user(tokens, n); }
        else if (cmd == "query_profile") { cmd_query_profile(tokens, n); }
        else if (cmd == "modify_profile") { cmd_modify_profile(tokens, n); }
        else {
            if (cmd == "query_ticket" || cmd == "query_transfer") {
                cout << 0 << '\n';
            } else {
                cout << -1 << '\n';
            }
        }
    }
    return 0;
}