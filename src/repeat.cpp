#include <dolores/dolores.hpp>
#include <rapidjson/document.h>

using namespace dolores::matchers;

dolores_on_message("问好", command("repeat")) {
    current.send(current.command_argument());
}