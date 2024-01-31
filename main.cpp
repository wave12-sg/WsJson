#include "wsJson.h"
using namespace SgLib;
#include <stdio.h>

int main(int argc, char *argv[]) {
     
    const char *json_str = "{\"name\":\"John\",\"age\":30,\"city\":{\"name\":\"New York\",\"country\":\"USA\",\"score\":[10, 20, 30]}}";

    WsJson objJson;
    T_JSON_NODE & root = objJson.parseJson(json_str);    
    switch (root.emType) {
        case JSON_NULL:
            printf("  Value: null\n");
            break;
        case JSON_BOOLEAN:
            printf("  Value: %s\n", root.toBoolean() ? "true" : "false");
            break;
        case JSON_NUMBER:
            printf("  Value: %f\n", root.toNumber());
            break;
        case JSON_STRING:
            printf("  Value: %s\n", root.toString());
            break;
        case JSON_ARRAY:
            printf("  Value: [array]\n");
            break;
        case JSON_OBJECT:
            printf("age: %f\n", root.get("age").toNumber()); 
            printf("city.country: %s\n", root.get("city").get("country").toString()); 
            printf("city.name: %s\n", root["city"]["name"].toString());    
            printf("city.score: %d\n", root["city"]["score"][1].toInt());             
            break;
    }
    return 0;
}