#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "json.h"

#if defined(WIN32) || defined(_WIN32)
#define PATH_SEPARATOR "\\"
#define JAVA_EXE "javaw.exe"
#else
#define PATH_SEPARATOR "/"
#define JAVA_EXE "java"
#endif

char *read_file(const char *file, size_t *size) {
  FILE *f = fopen(file, "r");
  if (!f) return NULL;
  fseek(f, 0, SEEK_END);
  size_t length = ftell(f);
  fseek(f, 0, SEEK_SET);
  char *buffer = (char *) malloc(length + 1);
  if (length && length != fread(buffer, 1, length, f)) {
    return NULL;
  }
  fclose(f);
  buffer[length] = '\0';
  *size = length;
  return buffer;
}

char *parse_username(char *json_path, char *access_token) {
  size_t size;
  char *json_content = read_file(json_path, &size);
  if (json_content == NULL) return NULL;
  struct json_value_s *root = json_parse(json_content, size);
  free(json_content);
  if (root->type != json_type_object) return NULL;
  struct json_object_s* rootObject = (struct json_object_s*) root->payload;
  struct json_object_element_s* authDb = rootObject->start;
  while (authDb != NULL && strcmp(authDb->name->string, "authenticationDatabase") != 0) {
    authDb = authDb->next;
  }
  if (authDb == NULL) return NULL;
  struct json_value_s *authDbValue = authDb->value;
  if (authDbValue->type != json_type_object) return NULL;
  struct json_object_s *authDbObject = (struct json_object_s *) authDbValue->payload;
  struct json_object_element_s *authProfile = authDbObject->start;
  while (authProfile != NULL) {
    struct json_value_s *authProfileValue = authProfile->value;
    if (authProfileValue->type == json_type_object) {
      struct json_object_s *authProfileObject = (struct json_object_s *) authProfileValue->payload;
      struct json_object_element_s *authProfileField = authProfileObject->start;
      char *json_access_token = NULL, *json_username = NULL;
      while (authProfileField != NULL) {
        if (strcmp(authProfileField->name->string, "accessToken") == 0) {
          if (authProfileField->value->type == json_type_string) {
            struct json_string_s* access_token_string = (struct json_string_s*)authProfileField->value->payload;
            json_access_token = (char *) malloc(access_token_string->string_size + 1);
            strcpy(json_access_token, access_token_string->string);
          }
        }
        if (strcmp(authProfileField->name->string, "username") == 0) {
          if (authProfileField->value->type == json_type_string) {
            struct json_string_s* username_string = (struct json_string_s*)authProfileField->value->payload;
            json_username = (char *) malloc(username_string->string_size + 1);
            strcpy(json_username, username_string->string);
          }
        }
        authProfileField = authProfileField->next;
      }
      if (json_access_token != NULL && json_username != NULL && strcmp(json_access_token, access_token) == 0) {
        free(json_access_token);
        return json_username;
      }
    }
    authProfile = authProfile->next;
  }
  return NULL;
}

int main(int argc, char *argv[]) {
  char *game_dir = NULL, *username = NULL, *access_token = NULL;
  int username_index = -1, demo_index = -1;
  for (int i = 0; i < argc; ++i) {
    if (strcmp(argv[i], "--username") == 0) {
      username = argv[i + 1];
      username_index = i + 1;
    } else if (strcmp(argv[i], "--gameDir") == 0) {
      game_dir = argv[i + 1];
    } else if (strcmp(argv[i], "--accessToken") == 0) {
      access_token = argv[i + 1];
    } else if (strcmp(argv[i], "--demo") == 0) {
      demo_index = i;
    }
  }

  if (game_dir == NULL) {
    printf("gameDir not defined\n");
    return 1;
  }
  if (username == NULL) {
    printf("username not defined\n");
    return 1;
  }

  if (strcmp(username, "Player") == 0) {
    char *json_file = "launcher_profiles.json";
    char json_path[strlen(game_dir) + strlen(PATH_SEPARATOR) + strlen(json_file) + 1];
    strcpy(json_path, game_dir);
    strcat(json_path, PATH_SEPARATOR);
    strcat(json_path, json_file);
    char *new_username = parse_username(json_path, access_token);
    if (new_username != NULL) {
      for (int i = 0; i < strlen(new_username); ++i) {
        if (new_username[i] == '@') {
          char* substring = (char *) malloc(i + 1);
          strncpy(substring, new_username, i);
          substring[i] = '\0';
          username = substring;
          break;
        }
      }
      free(new_username);
    }
  }

  int username_length = (int) strlen(username);
  char nickname[username_length + 1];
  strncpy(nickname, username, username_length);
  nickname[username_length] = '\0';
  free(username);

  argv[username_index] = nickname;
  argv[demo_index] = "";
  return execvp(JAVA_EXE, argv);
}
