#include "main.hpp"

int main() {
    const std::string encryption_key = "simple_key";
    const std::string filename = "../credentials.txt";
    
    CredentialEncryption ce;
    const std::string client_id     = ce.readDecryptedFromFile("client_id", encryption_key , filename);
    const std::string client_secret = ce.readDecryptedFromFile("client_secret", encryption_key, filename);

    App app(client_id, client_secret);
    app.menu();
    return 0;
}
