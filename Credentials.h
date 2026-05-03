// Provide all the crdentials here

// Your WiFi Credentials
const char *ssid = "*****";      // WiFi SSID Name
const char *password = "***************";// WiFi Password ( Keep it blank if your WiFi router is open )


// Parameters of Google Speech to Text API
const String ApiKey = "****************************";// Google Speech to Text API key
String SpeechtoText_Language = "en-IN"; // Language

// Parameters for GitHub Models API
// IMPORTANT: Use a GitHub Personal Access Token with "models:read" permissions.
// DELETE any old tokens that have been shared publicly and create a new one.
const char* GITHUB_MODELS_TOKEN = "**************************"; // <<-- REPLACE THIS

// The specific model you are using from the GitHub Marketplace
String GitHub_Model_ID = "***************"; 

// Model behavior parameters
String AI_Temperature = "0.7";              // Controls randomness: lower is more deterministic
String AI_Max_Tokens = "100";                 // Max length of the response
