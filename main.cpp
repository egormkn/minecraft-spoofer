#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "json.hpp"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#define PATH_SEPARATOR "\\"
#define JAVA_EXE "javaw.exe"
#else
#define PATH_SEPARATOR "/"
#define JAVA_EXE "java"
#endif

using namespace std;
using json = nlohmann::json;

string get_directory(const vector<string>& args) {
  auto gameDir = find(args.begin(), args.end(), "--gameDir");
  return gameDir == args.end() ? "." : *next(gameDir);
}

string get_username(const string& game_dir, const string& accessToken, const string& uuid) {
  json profiles_json;
  ifstream profiles(game_dir + PATH_SEPARATOR + "launcher_profiles.json");
  profiles >> profiles_json;
  string username = "Player";
  auto authenticationDatabase = profiles_json.find("authenticationDatabase");
  if (authenticationDatabase != profiles_json.end()) {
    for (auto& [key, value] : authenticationDatabase->items()) {
      cout << key << " : " << value << endl;
      auto email = value["username"].get<string>();
      cout << "Email: " << email << endl;
      auto jsonAccessToken = value["accessToken"].get<string>();
      cout << "Access token: " << jsonAccessToken << endl;
      if (jsonAccessToken == accessToken) {
        username = email.substr(0, email.find('@'));
        cout << "Setting username: " << username << endl;
        break;
      }
    }
  }
  profiles.close();
  return username;
}

int main(int argc, char * const argv[]) {
  // Get Java arguments
  vector<string> args(argv + 1, argv + argc);
  // Remove demo mode argument
  args.erase(remove(args.begin(), args.end(), "--demo"), args.end());
  // Find game directory
  string game_dir = get_directory(args);
  // Create log file
  ofstream logger(game_dir + PATH_SEPARATOR + "spoofer_log.txt");
  streambuf *coutbuf = cout.rdbuf();
  cout.rdbuf(logger.rdbuf());
  cout << "Starting log" << endl << "Running command: " << JAVA_EXE << ' ';
  for (auto& arg : args) {
    cout << arg << ' ';
  }
  cout << endl;
  // Get user information
  auto username = find(args.begin(), args.end(), "--username");
  if (username != args.end()) username = next(username);
  auto uuid = find(args.begin(), args.end(), "--uuid");
  if (uuid != args.end()) uuid = next(uuid);
  auto accessToken = find(args.begin(), args.end(), "--accessToken");
  if (accessToken != args.end()) accessToken = next(accessToken);
  // Replace username
  if (username != args.end() && *username == "Player") {
    string accessTokenString = accessToken == args.end() ? "0" : *accessToken;
    string uuidString = uuid == args.end() ? "0" : *uuid;
    *username = get_username(game_dir, accessTokenString, uuidString);
  }
  // Build new arguments
  stringstream command_builder;
  auto appender = ostream_iterator<string>(command_builder, " ");
  appender = JAVA_EXE;
  copy(args.begin(), args.end(), appender);
  string command = command_builder.str();
  cout << "Starting: " << command << endl;
  cout.rdbuf(coutbuf);
  logger.close();
  // Start game
  return system(command.c_str());
}
