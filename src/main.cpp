#include <array>
#include <bit>
#include <iostream>
#include <numeric>
#include <string_view>
#include <array>

constexpr uint64_t seed(const uint64_t seed_)
{
    uint64_t shifted = seed_;

    // one splitmix64 iteration
    shifted = (shifted += 0x9e3779b97f4a7c15);
	shifted = (shifted ^ (shifted >> 30)) * 0xbf58476d1ce4e5b9;
	shifted = (shifted ^ (shifted >> 27)) * 0x94d049bb133111eb;
	return shifted ^ (shifted >> 31);
}

// uses time of compilation for seeding rng
// good for compile-time randomness
constexpr uint64_t seed()
{
    uint64_t shifted = 0;
    for(const auto c : __TIME__)
    {
        shifted <<= 8;
        shifted |= c;
    }

	return seed(shifted);
}

struct xoroshiro128
{
    std::uint64_t state[2]{1, 1};

    xoroshiro128() = default;

    constexpr xoroshiro128(std::uint64_t s0, std::uint64_t s1) : state{s0, s1} {}

    // or c++20 std::rotl
    constexpr static inline uint64_t rotl(const uint64_t x, int k) 
    {
        return (x << k) | (x >> (64 - k));
    }

    constexpr std::uint64_t next() noexcept
    {
        const auto s0 = state[0];
        auto s1 = state[1];
        const auto result = rotl(s0 + s1, 17) + s0;

        s1 ^= s0;
        state[0] = rotl(s0, 49) ^ s1 ^ (s1 << 21);
        state[1] = rotl(s1, 28);

        return result >> 4;
    }

    template <typename T>
    constexpr T next_t() noexcept { return static_cast<T>(next()); }

    constexpr xoroshiro128 fork() { return xoroshiro128{next(), next()}; }
};

// from https://geckwiki.com/index.php?title=Hacking_Word_Count
enum class LockLevel
{
    VeryEasy = 0,
    Easy = 1,
    Average = 2,
    Hard = 3,
    VeryHard = 4
};

class yafth_engine_t
{
public:
    yafth_engine_t(const LockLevel lockLevelSetting_, const uint32_t playerScienceSkill_, const uint64_t seed_) 
        : rng(0, seed(seed_))
        , lockLevelSetting(lockLevelSetting_)
        , playerScienceSkill(playerScienceSkill_)
        , wordLength(set_word_length())
        , wordCount(set_word_count())
        , answer(rng.next() % wordCount)
    {
        generate_chars_stream();
        generate_words();
    }

    uint32_t get_word_count() const {return wordCount;}
    uint32_t get_word_length() const {return wordLength;}
    uint32_t get_attempts() const {return attemptsLeft;}

    const std::vector<std::string> & get_words() const {return words;}
    const std::array<char, 408> & get_chars_stream() const {return chars_stream;}

    const std::string & get_log() const {return log;}
private:
    uint32_t set_word_count();
    uint32_t set_word_length();
    void generate_chars_stream();
    void generate_words();

    xoroshiro128 rng;

    const LockLevel lockLevelSetting;
    const uint32_t playerScienceSkill;
    const uint32_t wordLength;
    const uint32_t wordCount;
    const uint32_t answer;

    uint32_t attemptsLeft = 4;
    std::string log = "";

    std::vector<std::string> words;
    std::array<char, 408> chars_stream;
    std::vector<decltype(chars_stream.begin())> words_pointers; // pointers to words in chars_stream
};

uint32_t yafth_engine_t::set_word_length()
{
    auto x = rng.next();
    std::cout << x << '\n';
    return std::min<uint32_t>(12, 4 + 2 * static_cast<uint32_t>(lockLevelSetting) + (x & 1));
}

uint32_t yafth_engine_t::set_word_count()
{
    // move this to input parser
    if(!(0 <= playerScienceSkill && playerScienceSkill <= 100))
    {
        throw std::runtime_error("Wrong player Science skill level!");
    }
    if(! (playerScienceSkill >= static_cast<uint32_t>(lockLevelSetting) * 25))
    {
        throw std::runtime_error("Player is not skilled enough to hack this terminal!");
    }

    const int32_t iHackingMinWords = 5;
    const int32_t iHackingMaxWords = 20;
    const int32_t lockLevel = (static_cast<uint32_t>(lockLevelSetting) * 0.25) * 100;
    const int32_t scienceOffset = playerScienceSkill - lockLevel;
    const int32_t lockOffset = 100 - lockLevel;

    int wordCount = 0;

    if (lockOffset == 0)
        wordCount = (0.5 * (iHackingMaxWords - iHackingMinWords)) + iHackingMinWords;
    else
        wordCount = ((1 - (1.0 * scienceOffset / lockOffset)) * (iHackingMaxWords - iHackingMinWords)) + iHackingMinWords;
    
    return std::min(20, wordCount);
}

void yafth_engine_t::generate_chars_stream()
{
    const std::array<char, 16> placeholders = 
        {'.', ',', '!', '?', '/', '*', '+', '\'', ':', ';', '-', '_', '%', '$', '|', '@'};
    for (auto & c : chars_stream){
        c = placeholders[rng.next() % 16];
    }
}

void yafth_engine_t::generate_words()
{
    words.reserve(wordCount);

    std::array<std::string, 100> wordArr{};
    if (wordLength == 4)
    {
        wordArr  = { "THAT", "WITH", "FROM", "WERE", "THIS", "THEY", "HAVE", "SAID", "WHAT", "WHEN", "BEEN", "THEM", "INTO", "MORE", "ONLY", "WILL", "THEN", "SOME", "TIME", "SUCH", "VERY", "OVER", "YOUR", "THAN", "WELL", "DOWN", "FACE", "UPON", "LIKE", "SAME", "KNOW", "WENT", "MADE", "LONG", "CAME", "ROOM", "MUST", "EVEN", "EYES", "COME", "MOST", "LIFE", "AWAY", "HAND", "LEFT", "ALSO", "JUST", "ARMY", "BACK", "GOOD", "HEAD", "MARY", "PART", "FELT", "HERE", "MUCH", "BONE", "TAKE", "MANY", "SKIN", "TOOK", "ONCE", "LOOK", "LAST", "BOTH", "GIVE", "SIDE", "FORM", "MAKE", "DOOR", "KNEW", "TELL", "TOLD", "WHOM", "LOVE", "DAYS", "DEAR", "SEEN", "GAVE", "CASE", "DONE", "FREE", "SOON", "EACH", "WORK", "LESS", "WIFE", "DOES", "MIND", "BODY", "OPEN", "WANT", "SENT", "HALF", "YEAR", "PAIN", "WORD", "HOME", "ANNA", "FIND" };
    }
    if (wordLength == 5)
    {
        wordArr  = { "WHICH", "THERE", "THEIR", "WOULD", "COULD", "AFTER", "OTHER", "ABOUT", "THESE", "THOSE", "FIRST", "WHERE", "UNDER", "STILL", "BEING", "AGAIN", "BEGAN", "GREAT", "ASKED", "WHILE", "COUNT", "WHOLE", "SHALL", "RIGHT", "PLACE", "STATE", "HOUSE", "EVERY", "HEARD", "YOUNG", "NEVER", "THREE", "YEARS", "THINK", "ROUND", "FOUND", "BLOOD", "POWER", "MIGHT", "SMALL", "QUITE", "LARGE", "VOICE", "WORDS", "HANDS", "CASES", "AMONG", "SONYA", "OFTEN", "TAKEN", "SMILE", "KNOWN", "ALONG", "ORDER", "WOMEN", "NIGHT", "STOOD", "JOINT", "CAUSE", "GOING", "GIVEN", "WORLD", "CHIEF", "FRONT", "WHITE", "ALONE", "HORSE", "LATER", "DEATH", "LABOR", "MONEY", "UNTIL", "WOMAN", "NERVE", "SOUTH", "WOUND", "THING", "LEAVE", "ABOVE", "ADDED", "PARTY", "PARTS", "TABLE", "BORIS", "ENEMY", "CRIED", "LIGHT", "UNION", "EARLY", "PETYA", "BONES", "SINCE", "HEART", "FORMS", "SPEAK", "MEANS", "MOVED", "FORCE", "THIRD", "SHORT" };
    }
    if (wordLength == 6)
    {
        wordArr  = { "PIERRE", "PRINCE", "BEFORE", "SHOULD", "ANDREW", "FRENCH", "LITTLE", "STATES", "PEOPLE", "ROSTOV", "LOOKED", "MOSCOW", "SEEMED", "HAVING", "THOUGH", "ALWAYS", "TISSUE", "UNITED", "FATHER", "DURING", "TURNED", "MOMENT", "HOLMES", "CALLED", "BATTLE", "OTHERS", "BECOME", "BEHIND", "COURSE", "RESULT", "PASSED", "MATTER", "ACTION", "TOWARD", "ALMOST", "THINGS", "TROOPS", "BECAME", "WITHIN", "MOTHER", "TAKING", "NUMBER", "EITHER", "LETTER", "PUBLIC", "COMMON", "FRIEND", "SECOND", "CANNOT", "GLANDS", "ITSELF", "REALLY", "TWENTY", "AROUND", "SAYING", "BETTER", "HORSES", "VASILI", "SYSTEM", "FORMED", "FELLOW", "LONGER", "MYSELF", "BEYOND", "GROWTH", "ANYONE", "ORDERS", "TUMOUR", "ACROSS", "RATHER", "BESIDE", "COMING", "MAKING", "OPENED", "WANTED", "RAISED", "FAMILY", "WISHED", "SEEING", "ANSWER", "EVENTS", "PERIOD", "GERMAN", "RUSSIA", "SPREAD", "MIDDLE", "REASON", "RETURN", "MERELY", "SILENT", "EFFECT", "GIVING", "WINDOW", "WOUNDS", "PERSON", "DINNER", "DOCTOR", "PLACED", "TRYING", "STREET" };
    }
    if (wordLength == 7)
    {
        wordArr  = { "NATASHA", "HIMSELF", "WITHOUT", "THOUGHT", "ANOTHER", "GENERAL", "THROUGH", "AGAINST", "BETWEEN", "NOTHING", "EMPEROR", "BECAUSE", "DISEASE", "KUTUZOV", "USUALLY", "LOOKING", "ALREADY", "CHAPTER", "OFFICER", "RUSSIAN", "HISTORY", "DENISOV", "HOWEVER", "COUNTRY", "BROUGHT", "PATIENT", "SEVERAL", "FEELING", "CERTAIN", "WHETHER", "HERSELF", "MORNING", "PRESENT", "REPLIED", "ENGLAND", "AMERICA", "TISSUES", "PROJECT", "EXAMPLE", "CARRIED", "ENTERED", "SURFACE", "SITTING", "BRITISH", "EVENING", "SHOUTED", "VESSELS", "BECOMES", "DRAWING", "FORWARD", "HUNDRED", "RESULTS", "WOUNDED", "SERVICE", "ANATOLE", "REACHED", "STRANGE", "PROCESS", "OPINION", "SOLDIER", "AFFAIRS", "ENGLISH", "SLAVERY", "STOPPED", "PERHAPS", "TURNING", "APPLIED", "MUSCLES", "TALKING", "RAPIDLY", "HUSBAND", "ABSCESS", "COMPANY", "NOTICED", "TUMOURS", "FEDERAL", "SUBJECT", "BELIEVE", "WAITING", "QUICKLY", "REMOVED", "ACCOUNT", "BROTHER", "FOREIGN", "GLANCED", "FREEDOM", "COVERED", "SOCIETY", "CHANGES", "NEITHER", "SMILING", "ROSTOVS", "SOMEONE", "VILLAGE", "LESIONS", "SPECIAL", "MEMBERS", "FINALLY", "FRIENDS", "VARIOUS" };
    }
    if (wordLength == 8)
    {
        wordArr  = { "PRINCESS", "AMERICAN", "NICHOLAS", "NAPOLEON", "COUNTESS", "SUDDENLY", "CONGRESS", "POSITION", "SOLDIERS", "ANYTHING", "QUESTION", "MOVEMENT", "POSSIBLE", "DOLOKHOV", "FOLLOWED", "BUSINESS", "OFFICERS", "NATIONAL", "RECEIVED", "TOGETHER", "THOUSAND", "EVERYONE", "PRESSURE", "CHILDREN", "REMAINED", "REGIMENT", "MILITARY", "ANSWERED", "INTEREST", "STANDING", "PRESENCE", "ANEURYSM", "REPEATED", "HAPPENED", "COLONIES", "AFFECTED", "APPEARED", "SOUTHERN", "TERRIBLE", "RETURNED", "FEATURES", "CAMPAIGN", "SPEAKING", "DAUGHTER", "STRENGTH", "COLONIAL", "MEASURES", "SYMPTOMS", "SYPHILIS", "GANGRENE", "REMARKED", "ALTHOUGH", "SWELLING", "ADJUTANT", "YOURSELF", "REMEMBER", "CLINICAL", "CONTRARY", "EMPLOYED", "LISTENED", "MEMBRANE", "PAVLOVNA", "RUSSIANS", "VIRGINIA", "PROBABLY", "COMPLETE", "GOVERNOR", "CARRIAGE", "DECLARED", "CONSISTS", "PLEASURE", "RELATION", "PREPARED", "ALPATYCH", "PROPERTY", "THINKING", "DISEASES", "INDUSTRY", "ATTENDED", "PRODUCED", "ACTIVITY", "OBSERVED", "DRESSING", "PEASANTS", "EXPECTED", "THOUGHTS", "STRAIGHT", "MAJORITY", "OCCURRED", "ECONOMIC", "COMMERCE", "ELECTION", "DISTANCE", "GENERALS", "HANDSOME", "OCCUPIED", "POLITICS", "SHOULDER", "BACTERIA", "INCREASE" };
    }
    if (wordLength == 9)
    {
        wordArr  = { "SOMETHING", "TREATMENT", "SOMETIMES", "EVIDENTLY", "PRESIDENT", "INFECTION", "CONDITION", "NECESSARY", "COMMANDER", "CONTINUED", "IMPORTANT", "DIFFERENT", "GUTENBERG", "POLITICAL", "FORMATION", "OPERATION", "FOLLOWING", "ATTENTION", "THEREFORE", "QUESTIONS", "BOLKONSKI", "CHARACTER", "ACCORDING", "BAGRATION", "ALEXANDER", "DESCRIBED", "RELATIONS", "DIFFICULT", "SECONDARY", "DIRECTION", "BEGINNING", "HAPPINESS", "INCREASED", "INFLUENCE", "EXPRESSED", "BOURIENNE", "DESTROYED", "EVERYBODY", "JEFFERSON", "ORGANISMS", "SHOULDERS", "RESULTING", "PRISONERS", "CERTAINLY", "PRESENTED", "ARTICULAR", "EXCLAIMED", "INTERESTS", "MOVEMENTS", "DIAGNOSIS", "TERRITORY", "SUFFERING", "VARIETIES", "CARTILAGE", "FRENCHMAN", "AUTHORITY", "DISCHARGE", "GENTLEMEN", "GENTLEMAN", "BEAUTIFUL", "DEMOCRACY", "WHISPERED", "BONAPARTE", "DEMOCRATS", "THOUSANDS", "DANGEROUS", "ESSENTIAL", "APPOINTED", "GENERALLY", "ROOSEVELT", "AMENDMENT", "GRADUALLY", "LISTENING", "INDICATED", "MALIGNANT", "SITUATION", "NECESSITY", "BRILLIANT", "INCLUDING", "AGITATION", "CONVINCED", "SOVEREIGN", "COMPANION", "CONCERNED", "SURPRISED", "AMERICANS", "SPERANSKI", "BENNIGSEN", "OURSELVES", "PROMINENT", "PARALYSIS", "SEPARATED", "HURRIEDLY", "AFFECTION", "COMPANIES", "SENSATION", "ABANDONED", "ADDRESSED", "CAREFULLY", "ARTHRITIS" };
    }
    if (wordLength == 10)
    {
        wordArr  = { "GOVERNMENT", "EVERYTHING", "UNDERSTAND", "ESPECIALLY", "EXPRESSION", "THEMSELVES", "IMPOSSIBLE", "PETERSBURG", "CONDITIONS", "UNDERSTOOD", "FREQUENTLY", "REVOLUTION", "ASSOCIATED", "REPUBLICAN", "WASHINGTON", "CONSIDERED", "CONVENTION", "SYPHILITIC", "FRIGHTENED", "APPEARANCE", "HISTORIANS", "EXCELLENCY", "ROSTOPCHIN", "REMEMBERED", "INDIVIDUAL", "IMPORTANCE", "DIFFICULTY", "EXPERIENCE", "DMITRIEVNA", "SURROUNDED", "CONNECTION", "CONNECTIVE", "POPULATION", "INDUSTRIAL", "OPPOSITION", "TENDERNESS", "COMPLETELY", "PARTICULAR", "AFFECTIONS", "FOUNDATION", "RECOGNIZED", "PRINCIPLES", "APPROACHED", "CONCEPTION", "DEMOCRATIC", "INTRODUCED", "OPERATIONS", "THROUGHOUT", "REMARKABLE", "PERIOSTEUM", "SUFFICIENT", "ADDRESSING", "AMPUTATION", "AFTERWARDS", "ENTERPRISE", "PROPORTION", "SETTLEMENT", "ABSOLUTELY", "COMPROMISE", "CHARACTERS", "INCREASING", "APPARENTLY", "COMMERCIAL", "CONTAINING", "IMPRESSION", "OCCURRENCE", "ACCUSTOMED", "HISTORICAL", "INTERESTED", "PARLIAMENT", "COMMANDERS", "DETERMINED", "INDUSTRIES", "IRRITATION", "PROTECTION", "RECOGNISED", "COMMISSION", "EXCITEMENT", "INEVITABLE", "EVERYWHERE", "CONCLUSION", "ELECTRONIC", "LEADERSHIP", "LEUCOCYTES", "RESOLUTION", "SUCCESSFUL", "CALIFORNIA", "EPITHELIUM", "POSSESSION", "PREVIOUSLY", "DIPLOMATIC", "CONFIDENCE", "PROTECTIVE", "ULCERATION", "UNPLEASANT", "ASSISTANCE", "HYPERAEMIA", "OCCUPATION", "PERMISSION", "PROVISIONS" };
    }
    if (wordLength == 11)
    {
        wordArr  = { "TUBERCULOUS", "IMMEDIATELY", "HAEMORRHAGE", "SUPPURATION", "REPUBLICANS", "MIKHAYLOVNA", "ESTABLISHED", "DEVELOPMENT", "DESTRUCTION", "INTERRUPTED", "TEMPERATURE", "EXPERIENCED", "SURROUNDING", "SUPERFICIAL", "CIRCULATION", "CONTINUALLY", "POSSIBILITY", "ACCOMPANIED", "GRANULATION", "INHABITANTS", "LEGISLATURE", "MISSISSIPPI", "APPLICATION", "INDEPENDENT", "INFORMATION", "EXAMINATION", "INTERESTING", "APPROACHING", "IMMIGRATION", "OPPORTUNITY", "CONTRACTION", "LEGISLATION", "DISAPPEARED", "EXPLANATION", "AGRICULTURE", "AUTHORITIES", "ENLARGEMENT", "CONSIDERING", "FEDERALISTS", "PROGRESSIVE", "TRANSFERRED", "CONNECTICUT", "DECLARATION", "IMAGINATION", "BOGUCHAROVO", "EPITHELIOMA", "CONTINENTAL", "ASSOCIATION", "COMBINATION", "TERRITORIES", "DISTURBANCE", "REPRESENTED", "CICATRICIAL", "DISTRIBUTED", "SENSIBILITY", "APPOINTMENT", "COMPRESSION", "OBSERVATION", "SIGNIFICANT", "DISLOCATION", "INNUMERABLE", "BATTLEFIELD", "DISPOSITION", "OBLIGATIONS", "EXCEPTIONAL", "RESTORATION", "UNFORTUNATE", "ARRANGEMENT", "RECOGNIZING", "CONFEDERATE", "CORPORATION", "DISTINGUISH", "GOVERNMENTS", "CONTROVERSY", "INQUIRINGLY", "PREPARATION", "RECOGNITION", "APPROPRIATE", "ATTENTIVELY", "EXCEEDINGLY", "IMPROVEMENT", "INDIVIDUALS", "PRACTICALLY", "RESPONSIBLE", "APPEARANCES", "INDIFFERENT", "LEGISLATIVE", "NECESSARILY", "RECOMMENDED", "SUPPURATIVE", "UNDOUBTEDLY", "CONFEDERACY", "CONNECTIONS", "UNNECESSARY", "COMPLICATED", "CONVENTIONS", "PHILIPPINES", "CONSEQUENCE", "CONSTITUTES", "CONTRACTURE" };
    }
    if (wordLength == 12)
    {
        wordArr  = { "CONSTITUTION", "ILLUSTRATION", "CONVERSATION", "PARTICULARLY", "CONSIDERABLE", "INDEPENDENCE", "MADEMOISELLE", "SUBCUTANEOUS", "PENNSYLVANIA", "INFLAMMATION", "OCCASIONALLY", "SIGNIFICANCE", "INTERFERENCE", "TUBERCULOSIS", "LEGISLATURES", "SATISFACTION", "PHILADELPHIA", "DEGENERATION", "DISTRIBUTION", "GRANULATIONS", "UNEXPECTEDLY", "ACQUAINTANCE", "HANDKERCHIEF", "DIFFICULTIES", "INFLAMMATORY", "HEADQUARTERS", "INSTRUCTIONS", "PROCLAMATION", "CONSERVATIVE", "PREPARATIONS", "MANUFACTURES", "SURROUNDINGS", "ORGANIZATION", "RESPECTFULLY", "SATISFACTORY", "CIVILIZATION", "INTELLECTUAL", "ACCOMPLISHED", "LEUCOCYTOSIS", "NEVERTHELESS", "RATIFICATION", "REGENERATION", "OSSIFICATION", "PATHOLOGICAL", "DISPOSITIONS", "DISSATISFIED", "INSTITUTIONS", "COMBINATIONS", "NEGOTIATIONS", "PRESIDENTIAL", "SUFFICIENTLY", "CONCENTRATED", "INTRODUCTION", "ARRANGEMENTS", "CONSEQUENTLY", "ASTONISHMENT", "PROFESSIONAL", "IMPROVEMENTS", "CORPORATIONS", "DISPLACEMENT", "INDIFFERENCE", "SUBSEQUENTLY", "ACCUMULATION", "CONSTRUCTION", "INTERVENTION", "SUCCESSFULLY", "FIBROMATOSIS", "INFILTRATION", "VERESHCHAGIN", "AFFECTIONATE", "APPLICATIONS", "DELIBERATELY", "EMANCIPATION", "LYMPHANGITIS", "PASSIONATELY", "RECOLLECTION", "REPRESENTING", "RESTRICTIONS", "AGRICULTURAL", "ESTABLISHING", "IRRESISTIBLE", "ANNOUNCEMENT", "CONGRATULATE", "DEMONSTRATED", "DIFFERENTIAL", "HENDRIKHOVNA", "REQUIREMENTS", "ACCIDENTALLY", "MILORADOVICH", "COMMENCEMENT", "COMPENSATION", "DISTRIBUTING", "STRENGTHENED", "TRANQUILLITY", "ACCOMPANYING", "ADMINISTERED", "ADVANTAGEOUS", "COLONIZATION", "CONSEQUENCES", "CONSIDERABLY" };
    }
    uint32_t i = 0;
    do
    {
        auto nextWord = wordArr[rng.next() % 100];
        if(std::find(words.begin(), words.end(), nextWord) == words.end())
        {
            words.push_back(nextWord);
            ++i;
        }
    } while (i < wordCount);
}

int main()
{
    yafth_engine_t yet(LockLevel::Easy, 65, time(0));
    const auto arr = yet.get_chars_stream();
    const auto len = yet.get_word_length();
    const auto count = yet.get_word_count();
    const auto words = yet.get_words();

    std::cout << len << '\n';
    std::cout << count << '\n';
    std::cout << arr[407] << '\n';
    std::cout << words[count-1] << '\n';

    xoroshiro128 rng{1, seed()};
    auto answer = rng.next() % count;

    for(uint32_t i = 0; i < count; ++i)
    {
        int similarity = 0;
        for(uint32_t j = 0; j < len; ++j)
            similarity += (words[i][j] == words[answer][j]);

        std::cout << words[i] << ' ' << similarity << '/' << len << '\n';
    }

    return 0;
}