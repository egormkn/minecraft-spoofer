#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>

#include "json.hpp"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#define PATH_SEPARATOR "\\"
#define JAVA_EXE "javaw.exe"
#else
#define PATH_SEPARATOR "/"
#define JAVA_EXE "java"
#endif

#define DEFAULT_ACCESS_TOKEN "0"
#define DEFAULT_USERNAME "Player"
#define DEFAULT_UUID "0"

using namespace std;
using json = nlohmann::json;

string get_directory(const vector<string>& args) {
  auto gameDir = find(args.begin(), args.end(), "--gameDir");
  return gameDir == args.end() ? "." : *next(gameDir);
}

string get_username(const string& game_dir, const string& accessToken, const string& uuid) {
  json profiles_json;
  string profiles_path = game_dir + PATH_SEPARATOR + "launcher_profiles.json";
  ifstream profiles(profiles_path);
  profiles >> profiles_json;
  cout << "Parsed json file: " << profiles_path << endl;
  string username = DEFAULT_USERNAME;
  auto authenticationDatabase = profiles_json.find("authenticationDatabase");
  if (authenticationDatabase != profiles_json.end()) {
    cout << "authenticationDatabase object is found" << endl;
    for (auto& [key, value] : authenticationDatabase->items()) {
      cout << key << " : " << value << endl;
      auto email = value["username"].get<string>();
      cout << "Email: " << email << endl;
      auto jsonAccessToken = value["accessToken"].get<string>();
      cout << "Access token: " << jsonAccessToken << endl;
      if (accessToken == DEFAULT_ACCESS_TOKEN || jsonAccessToken == accessToken) {
        username = email.substr(0, email.find('@'));
        cout << "Setting username: " << username << endl;
        break;
      }
    }
  } else {
    cout << "authenticationDatabase object was not found" << endl;
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
  string logger_path = game_dir + PATH_SEPARATOR + "spoofer_log.txt";
  ofstream logger(logger_path);
  streambuf *coutbuf = cout.rdbuf();
  cout.rdbuf(logger.rdbuf());
  cout << "Starting log" << endl << endl << "Original arguments:" << endl;
  for (auto& arg : args) {
    cout << arg << endl;
  }
  cout << endl << endl;
  // Get user information
  auto username = find(args.begin(), args.end(), "--username");
  if (username == args.end()) {
    username = find(args.begin(), args.end(), "net.minecraft.launchwrapper.Launch");
    cout << "Using legacy username from net.minecraft.launchwrapper.Launch args" << endl;
  }
  if (username != args.end()) username = next(username);
  auto uuid = find(args.begin(), args.end(), "--uuid");
  if (uuid != args.end()) uuid = next(uuid);
  auto accessToken = find(args.begin(), args.end(), "--accessToken");
  if (accessToken != args.end()) accessToken = next(accessToken);
  // Replace username
  if (username != args.end() && *username == DEFAULT_USERNAME) {
    cout << "Replacing username..." << endl;
    string accessTokenString = accessToken == args.end() ? DEFAULT_ACCESS_TOKEN : *accessToken;
    string uuidString = uuid == args.end() ? DEFAULT_UUID : *uuid;
    *username = get_username(game_dir, accessTokenString, uuidString);
  }
  // Build new arguments
  stringstream command_builder;
  auto appender = ostream_iterator<string>(command_builder, " ");
  appender = JAVA_EXE;
  for_each(args.begin(), args.end(), [](string &arg) {
    if (arg.find(' ') != std::string::npos) {
      arg = "\"" + arg + "\"";
    }
  });
  copy(args.begin(), args.end(), appender);
  string command = command_builder.str();
  cout << endl << "Starting: " << command << endl;
  cout.rdbuf(coutbuf);
  logger.close();
  // Start game
  return system((command  + " >> " + logger_path).c_str());
}
