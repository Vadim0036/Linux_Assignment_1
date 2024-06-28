#include <iostream>
#include <string>
#include <fstream>
using namespace std;


// REQUIRED DEFAULT FUNCTIONS 
void read_questions(string path);                                                                       // Reads in the questions/responses from file, storing to 2D array of strings

void read_answers(string path);                                                                         // Reads correct answers letter for each question, storing to 1D array of chars

void play_game(int seed);                                                                               // Manages the entire game and invokes other function's to run the quiz 

void show_question(int question_number, string player_name, bool second_try, char player_response);     // Displays the question and responses, labeled A-D, optionally hiding the specified response for the second try 

void player_try(int question_number, string player_name, bool* is_second_miss, int* number_scores);     // Manages the users input checks if the answer correct or not 

void sort_score();                                                                                      // Sorts the players scores based on numbers of points and save data to summary txt file


// ADDITIONAL FUNCTIONS
void select_random_questions(int seed);                                    // Validates and picks 6 random questions collecting them in 1D random_questions[] array

bool is_player_correct(int question_number, char player_response);         // Determines whether user answered question correctly or not

bool offer_second_try();                                                   // Offers to skip the question or try it second time, in case of agreement to try second time function returns true otherwise false

void score_player(bool second_try, bool isCorrect, int* number_score);     // Scores the player based on the performence 

bool validate_input_files();                                               // Validates the input files, so files match program requirements

void record_player_result(int score, string user_name);                    // Appands new player and the score to the summary text file

void show_leaderboard();                                                   // After game is over, this function will show the score board taken from summary.txt file

void determine_position(string name, int points);                          // At the end of the game determines players position on leader board

bool isLineEmpty(string current_line);                                     // Checks if the current line of the file empty or not 

string toUpper(string text);                                               // Used as collateral function to transform low case string into upper


// GLOBAL VARIABLES
const int NUMBER_QUESTIONS = 50;                                           // Number of questions sets in text file ( Global Variable )
const int NUMBER_ANSWERS =  NUMBER_QUESTIONS;                              // Number of answers in answers text file  ( Global Variable )

string questions[NUMBER_QUESTIONS][5];                                     // 2D array that contains questions and responses ( Global Variable )
char answers[NUMBER_ANSWERS];                                              // 1D array that contains correct answers ( Global Variable )
int random_questions[6];                                                   // 1D array that contains 6 random integers which represent 6 random indexes of questions picked and validated by function select_random_question();

int scores[6] = {0,0,0,0,0,0};                                             // 1D array that contains scores for each answered question; it is used to easily calculate player score at the end of the quiz


int main(int argc, char* argv[]){
    // Checking for correct number of arguments
    if(argc != 4){
        cout << "Incorrect program usage." << endl;
        cout << "Example of usage -> ./progName question.txt(path) answers.txt(path) seed " << endl; 
        cout << "Please read the program description and try again. Exiting." << endl;
        return -1;
    }

    // Set variables 
    string question_path = argv[1];     
    string answers_path = argv[2];
    int seed = atoi(argv[3]);

    read_questions(question_path);
    read_answers(answers_path);

    if(!validate_input_files())
        return -1;

    play_game(seed);

    return 0;
}

bool validate_input_files(){
    int number_questions = 0;
    int number_answers = 0;
    // iteretes through each txt file line 
    for(int i = 0; i < 50; ++i){
        if(questions[i][0] != "")   // checks if line, in question array, is empty or contains question
            number_questions++;
        if(answers[i] != '\0')     // checks if line, in answer array, is empty or contains response letter
            number_answers++;
    }
    
    // checks if qustion array or answer array are empty
    if(number_answers == 0 || number_questions == 0){
        cout << "The question file or the answer file is empty." << endl;
        cout << "Please check the files and try again. Exiting." << endl;
        return false;
    }
    // checks if number of questions is equivalent to number of answers
    else if(number_questions =! number_answers){
        cout << "The question and answer files have different number of items." << endl;
        cout << "Please check the files and try again. Exiting." << endl;
        return false;
    }
    // if everything looks good - function returns true
    else 
        return true;
}

void read_questions(string path){

    ifstream iFile; 
    iFile.open(path);
    if(!iFile.is_open()){
        cout << "Could not open the file questions.txt" << endl;
        return; 
    }

    string letter_responses[] = {"", "A. ", "B. ", "C. ", "D. "};   // coressponding letters for answers choices

    // parse the question sets from txt file into 2D Array
    unsigned int num_question = 0;
    unsigned int num_answer = 0;
    string current_line;
    while(getline(iFile,current_line)){
        if(isLineEmpty(current_line)){
            num_question ++;
            num_answer = 0;
        }
        else{
            questions[num_question][num_answer] = letter_responses[num_answer] + current_line;   // adding coressponding letter to answer choices
            num_answer ++;
        }
    }
    iFile.close();
}

void read_answers(string path){
    ifstream iFile;
    iFile.open(path);
    if(!iFile.is_open()){
        cout << "Could not open the file answers.txt" << endl;
        return;
    }

    // parse all answers from txt file into 1D Array
    unsigned int answer_num = 0;
    while(!iFile.eof()){
        char current_answer;
        iFile >> current_answer;
        answers[answer_num] = current_answer;
        answer_num ++;
    }
    iFile.close();
}

void select_random_questions(int seed){
    int rand_num;
    bool check[50];

    // if question is valid for selection - false is assigned; if question is not valid for selection - true is assigned
    for(int i = 0; i < 50; i++){
        // question can't be empty line 
        if(questions[i][0] != "")
            check[i] = false;
        else
            check[i] = true;
    }
    srand(seed);
    // iterates until 6 indexes for questions are selected
    for(int i = 0; i < 6; ++i){
        rand_num = rand() % 50;   // generate random question
        while(check[rand_num])  // until true
            rand_num = rand() % 50;
        check[rand_num] = true;  // to avoid repetition of questions
        random_questions[i] = rand_num;  // random index appends to ran_question array 
    }
}

void play_game(int seed){
    // selecting random questions
    select_random_questions(seed); 

    int final_score = 0;  

    string player_name;         
    cout << "What's your name?>";
    cin >> player_name;

    int question_number = 0;      // the number of question being printed to the player 
    bool is_second_miss = false;  // becomes true if player makes second mistake and game terminates
    int number_scores = 0;        // position in score array which is used to calculate the final score after the game
    
    bool is_end = false;         // boolean variable represents whether the game is finished 
    for (int i = 0; i < 6; ++i){ 
        player_try(question_number, player_name, &is_second_miss, &number_scores);
        
        if (is_second_miss == true)
            break;  // Exit the loop if a second miss occurs.

        question_number++;
    }

    // Summing scores from each question to get final score
    for(int i = 0; i < 6; ++i){
        if(is_second_miss){
            final_score = 0;
            break;
        }
        final_score += scores[i];
    }
    cout << "That's the end of the game, there are no more questions!" << endl;
    cout << endl;
    cout << player_name << " Your final score was: " << final_score << endl;
    cout << "This will be recorded to the game history file." << endl;
    record_player_result(final_score, player_name);   // appends player's result to the summary file
    sort_score();                                     // sorts the summary file 
    show_leaderboard();                               // shows the leader of the summary file 
    determine_position(player_name, final_score);     // shows the current_player position on leaderboard based on performence 
    cout << "Thanks for playing, we hope you'll send all your friends to play." << endl;
}

void player_try(int question_number, string player_name, bool* is_second_miss, int* number_scores){
    bool second_try = false;                    // indicates if player answers the question for the second time 
    bool is_player_response_correct = false;   // indicated if player answers corretly on last question
    char player_response = ' ';                // temp variable representing player's response

    // Iterates until player fails twice, or player answers to the question correctly 
    while(!is_player_response_correct){
        show_question(question_number, player_name, second_try, player_response);  // shows question to the player
        cout << "Your choice?> ";
        cin >> player_response;                                     // saves player's response 
        player_response = toupper(player_response);                 // makes player's response case unsensitive

        if(is_player_correct(question_number, player_response) == true){
            is_player_response_correct = true;
        }
        else if(second_try == true){   
            *is_second_miss = true;   // this variable terminates the game if second attempt is missed 
            break;
        }
        else if(offer_second_try() == false) // offers second try if declined - loop teminates and user goes to next queston, otherwise player gets second attmept
            break;
        else
            second_try = true;
    }
    score_player(second_try, is_player_response_correct, number_scores);  // gives player points for the question 
}

void show_question(int question_number, string player_name, bool second_try, char player_response){
    // checks if the question is answered for the second time
    if(!second_try)
        cout << player_name << " Here's Question Number " << (question_number + 1) << endl;
    else 
        cout << player_name << " Here's Question Number " << (question_number + 1) << " (second try)" << endl;  

    // iterates through certain question and answers in array
    unsigned int i = 0;
    for(i = 0; i < 5; ++i){
        string current_line = questions[random_questions[question_number]][i];
        if(current_line.at(0) == player_response)
            continue;
        else
            cout << questions[random_questions[question_number]][i] << endl;
    }
}

bool is_player_correct(int question_number, char player_response){
    //determines if player's answer is correct or not
    if(answers[random_questions[question_number]] == player_response)
        return true;
    else 
        return false;
}

bool offer_second_try(){
    string user_choice; 
    cout << "Do you want second try or you want to skip. Type /skip/ to skip or type /try/ to get second try" << endl;
    cin >> user_choice;
    user_choice = toUpper(user_choice);

    // Checks users response
    if(user_choice == "SKIP")
        return false;
    else if(user_choice == "TRY")
        return true;
    // if user reply with something different than skip or try, program automatically skips the question
    else{
        cout << "You entered wrong choice. So programm skips the second try!!!" << endl;
        return false;
    }
}

void score_player(bool second_try, bool isCorrect, int* number_scores){ 
   int current_question = *number_scores;
    // if question is first and answered correctly using second attempts - user gets 5 points
   if(current_question == 0 && isCorrect && second_try){
        scores[current_question] = 5;  // points for certain question is appended to global score_array
        cout << "You got that one right, for 5 points" << endl;
        cout << endl;
        (*number_scores)++;
   } 
   // if question is first and answered correctly from first attempt - user gets 10 points
   else if(current_question == 0 && isCorrect){
        scores[current_question] = 10;  // points for certain question is appended to global score_array
        cout << "You got that one right, for 10 points" << endl;
        cout << endl;
        (*number_scores)++;
   }
   // if question is answered using second attempt correctly - user gets previous score * 5 points 
   else if(second_try && isCorrect){
        scores[current_question] = scores[current_question -1] * 5;  // points for certain question is appended to global score_array
        cout << "You got that one right, for " << scores[current_question] << " points!!!" << endl;
        cout << endl;
        (*number_scores)++;
   }
   // if question is answered incorrectly - player gets 0 points and function returns 
   else if(!isCorrect){
        return;
   }
   // if user answered question from first attempt, user - gets prevous score * 10 points
   else{
        scores[current_question] = scores[current_question -1 ] * 10;  // points for certain question is appended to global score_array
        cout << "You got that one right, for " << scores[current_question] << " points!!!" << endl;
        cout << endl;
        (*number_scores)++;
   }
}

void sort_score(){
    int number_players = 0;
    
    // at first, function checks how many players in summary.txt file
    ifstream summary;
    summary.open("summary.txt");
    if (!summary.is_open()) {
        cout << "summary.txt file can't be opened. Exiting!" << endl;
        return;
    }
    string line;
    while (getline(summary, line)) {
        if (!line.empty())
            number_players++;
    }
    summary.close();

    string player_result[number_players][2]; // Temporary 2D array that holds scores and player's names

    // parsing information from summary.txt file into 2D array 
    ifstream sum;
    sum.open("summary.txt");
    if (!sum.is_open()) {
        cout << "summary.txt file can't be opened. Exiting!" << endl;
        return;
    }

    bool score = false;
    string data;
    int i = 0;
    while (sum >> data) {
        if (!score) {
            player_result[i][0] = data;
            score = true;
        } else {
            player_result[i][1] = data;
            score = false;
            i++;
        }
    }
    sum.close();

    // Sorting an array with scores and players
    for (int i = 0; i < number_players - 1; i++) {
        int maxIdx = i;

        for (int j = i + 1; j < number_players; j++) {
            int score1 = stoi(player_result[j][1]);
            int score2 = stoi(player_result[maxIdx][1]);

            if (score1 > score2) {
                maxIdx = j;
            }
        }

        swap(player_result[i][0], player_result[maxIdx][0]);
        swap(player_result[i][1], player_result[maxIdx][1]);
    }

    // writing sorted array into summary.txt file
    ofstream save;
    save.open("summary.txt");
    if (!save.is_open()) {
        cout << "Unable to write in summary.txt to save data. Exiting" << endl;
        return;
    }
    for (int i = 0; i < number_players; i++) {
        save << player_result[i][0] << "     " << player_result[i][1] << endl; // Fixed the typo here
    }
    save.close();

    // I know, this function could be written in a more efficient manner, but I did not have enough time to work on it properly.
}

void show_leaderboard(){
    ifstream summary;
    summary.open("summary.txt");
    if(!summary.is_open()){
        cout << "Unable to open summary.txt file, Exiting." << endl;
        return;
    }
    string name;
    string score;
    summary >> name;  // gets name from summary.txt file of a player with the highest score
    summary >> score;  // gets score from summary.txt file of a player with the highest score
        
    cout << "The current record holder is " << name << " with " << score << " points." << endl;
}

void determine_position(string name, int points){
    bool is_name = true;   // when reading file, is_name variable allows to determine if current line is score or it is name
    int position = 1;      // position of a player based on points 
    ifstream summary;
    summary.open("summary.txt");
    if(!summary.is_open()){
        cout << "File cant be opened, try again. Exiting!!!" << endl;
        return;
    }
    string line;
    // iterates each line of a text file until reaches the end of a file
    while(!summary.eof()){
        summary >> line;
        // if given name matches with name in file, loop is terminated 
        if(name == line) 
            break;
        // if name does not match with name in file, loop iterates and position variable increments by 1
        else if(!isdigit(line.at(0))){
            position ++;
        }
    }
    summary.close();  // closes file
    cout << "You have achived rank " << position << " with " << points << " points." << endl;
}

void record_player_result(int score, string user_name){
    ofstream summary;  // facilitates ofstream
    summary.open("summary.txt", ios::app);  // opens file
    // checks if given file is opened 
    if(!summary.is_open()){
        cout << "The file summary.txt is in read-only mode, change the properties and try again!!!" << endl;
        return; 
    }
    summary << user_name << "     " << score << endl;  // writes a new user statistics to file
    summary.close(); // closes file stream
}

string toUpper(string text){
    string final_string = "";
    // Iterates each char for given string
    for(int i = 0; i < text.size(); i++){
        final_string += toupper(text.at(i));  // Rises given char into upper case
    }
    return final_string;
}

bool isLineEmpty(string current_line) {
    // Iterates each char for given string 
    for(int i = 0; i < current_line.size(); i++){
        char current_char = current_line.at(i);
        // Checks if given char is empty space
        if(!isspace(current_char))
            return false;
    }
    return true;
} 