#include <dolores/dolores.hpp>

using namespace dolores::matchers;

dolores_on_message("问好", command(".repeat")) {
    current.send(current.command_argument());
}