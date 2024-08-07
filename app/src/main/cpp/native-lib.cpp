#include <jni.h>
#include <string>
#include <torch/script.h>
#include <android/log.h>
#include <torch/script.h>
#include <torch/csrc/jit/api/module.h>
#include <iostream>
#include <fstream>
#include <memory>
#include <vector>

#include "ATen/ATen.h"
#include "torch/csrc/autograd/grad_mode.h"
#include "torch/script.h"
#include <torch/csrc/jit/mobile/function.h>
#include <torch/csrc/jit/mobile/import.h>
#include <torch/csrc/jit/mobile/interpreter.h>
#include <torch/csrc/jit/mobile/observer.h>
#include "opencv2/opencv.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "fastBPE.hpp"
#include <sstream>
#include <unordered_map>
#include "npy.hpp"
#include <c10/core/TensorOptions.h>

using namespace std;
using namespace fastBPE;
using Dict = std::map<int, int>;

static torch::jit::mobile::Module model;
static torch::jit::mobile::Module commonClassifier_model;
static torch::jit::mobile::Module number_student_model;
static torch::jit::mobile::Module color_student_model;
static bool isModelLoaded = false;



std::unordered_map<std::string, int> loadVocab(const std::string& filename) {
    std::unordered_map<std::string, int> vocab;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Không thể mở file " << filename << std::endl;
        return vocab;
    }

    std::string line;
    bool inVocabSection = false;
    while (std::getline(file, line)) {
        // Tìm phần bắt đầu của "vocab" trong JSON
        if (line.find("\"vocab\": {") != std::string::npos) {
            inVocabSection = true;
            continue;
        }

        // Kết thúc phần "vocab"
        bool found = (line.find("},") != std::string::npos) ? true : false;
        if (inVocabSection && found) {
            file.close();
            break;
        }
        // Xử lý các dòng trong phần "vocab"
        if (inVocabSection) {
            int colonPosFirst = line.find('"');
            size_t colonPos = line.find_last_of('"');

            if (colonPos != std::string::npos) {
                std::string key = line.substr(colonPosFirst + 1, colonPos - colonPosFirst - 1); // Bỏ dấu ngoặc kép
                key.erase(0, key.find_first_not_of(" \t")); // Xóa khoảng trắng đầu
                key.erase(key.find_last_not_of(" \t") + 1); // Xóa khoảng trắng cuối

                std::string valueStr = line.substr(colonPos + 2);
                valueStr.erase(0, valueStr.find_first_not_of(" \t")); // Xóa khoảng trắng đầu
                valueStr.erase(valueStr.find_last_not_of(" \t,") + 1); // Xóa khoảng trắng và dấu phẩy cuối

                int value = std::stoi(valueStr);
                vocab[key] = value;
            }
        }
    }

    // file.close();
    return vocab;
}

int checkQuestion(const std::string& rawQuestion) {
    const std::vector<std::string> listKey = {/* Các từ khóa của bạn ở đây */};
    const std::unordered_map<std::string, int> typeDict = {/* Bản đồ loại của bạn ở đây */};

    int output = -1;
    bool isKeyInText = false;

    // Vòng lặp chính kiểm tra từ khóa
    for (const std::string& key : listKey) {
        if (key != "gì" && rawQuestion.find(key) != std::string::npos) {
            output = typeDict.at(key);  // Sử dụng at() để an toàn hơn
            isKeyInText = true;
            break;
        }
    }

    // Xử lý trường hợp "gì" riêng biệt
    if (!isKeyInText && rawQuestion.find("gì") != std::string::npos) {
        if (rawQuestion.find("màu") != std::string::npos) {
            output = 2;
        } else {
            output = 0;
        }
        isKeyInText = true;
    }

    // Kiểm tra các từ khóa còn lại
    for (const std::string& key : {"nào", "nơi", "thứ"}) {
        if (rawQuestion.find(key) != std::string::npos) {
            output = (key == "nơi") ? 3 : 0;
            isKeyInText = true;
            break;
        }
    }

    // Nếu không tìm thấy từ khóa nào, mặc định là loại 0
    return (output == -1) ? 0 : output;
}

int m_argmax(const torch::Tensor& input) {

    // Lấy index của giá trị lớn nhất trên dimension 1 (cột)
    auto max_result = torch::argmax(input, 1);

    // Chuyển đổi kết quả sang kiểu int và trả về
    return max_result.item<int>();
}



std::map<int, std::string> label_map = {
        {0, "đường đi bộ"},
        {1, "táo"},
        {2, "con ngựa"},
        {3, "quả bóng"},
        {4, "cà rốt"},
        {5, "cá"},
        {6, "khăn"},
        {7, "mái chèo"},
        {8, "donut"},
        {9, "lồng"},
        {10, "khăn tắm"},
        {11, "chai"},
        {12, "vali"},
        {13, "dĩa"},
        {14, "ba lan"},
        {15, "cửa tiệm"},
        {16, "hay"},
        {17, "bãi cỏ"},
        {18, "hươu cao cổ"},
        {19, "cây sào"},
        {20, "màu đỏ"},
        {21, "cà vạt"},
        {22, "mắt"},
        {23, "bếp"},
        {24, "đường bộ"},
        {25, "cupcake"},
        {26, "ba"},
        {27, "chung cư"},
        {28, "nến"},
        {29, "bữa ăn"},
        {30, "ghế sô pha"},
        {31, "chảo"},
        {32, "lăn tròn"},
        {33, "cừu"},
        {34, "món tráng miệng"},
        {35, "gà"},
        {36, "bàn chải"},
        {37, "bức ảnh"},
        {38, "bàn chải đánh răng."},
        {39, "cái rổ"},
        {40, "sân bay"},
        {41, "xe lăn"},
        {42, "các tòa nhà"},
        {43, "tủ lạnh"},
        {44, "ký tên"},
        {45, "con mèo"},
        {46, "bốn"},
        {47, "cờ"},
        {48, "áo khoác"},
        {49, "đường phố"},
        {50, "bờ biển"},
        {51, "bức tượng"},
        {52, "chìm xuống"},
        {53, "vạch kẻ đường"},
        {54, "đồ chơi"},
        {55, "tàu hỏa"},
        {56, "màu sắc"},
        {57, "sandwich"},
        {58, "bến du thuyền"},
        {59, "giá đỡ"},
        {60, "chuối"},
        {61, "gia súc"},
        {62, "dấu hiệu"},
        {63, "cam"},
        {64, "bánh"},
        {65, "bồn tắm"},
        {66, "cỗ máy"},
        {67, "búi tóc"},
        {68, "màu vàng"},
        {69, "điểm tâm"},
        {70, "bia"},
        {71, "túi"},
        {72, "sàn nhà"},
        {73, "cái mâm"},
        {74, "đĩa"},
        {75, "xe"},
        {76, "truyền hình"},
        {77, "cái nồi"},
        {78, "bánh hamburger"},
        {79, "màu đen"},
        {80, "tủ đông"},
        {81, "xe máy"},
        {82, "bánh xe"},
        {83, "nhiều cái ghế"},
        {84, "chim ưng"},
        {85, "bò"},
        {86, "cây kéo"},
        {87, "thùng chứa"},
        {88, "gầu múc"},
        {89, "cửa"},
        {90, "hộp số"},
        {91, "vườn bách thú"},
        {92, "cái túi"},
        {93, "núi"},
        {94, "bảo tàng"},
        {95, "chuột"},
        {96, "bộ đồ"},
        {97, "bữa trưa"},
        {98, "con chim"},
        {99, "dĩa nhựa"},
        {100, "chuồng trại"},
        {101, "gậy"},
        {102, "ngăn kéo"},
        {103, "bông hoa"},
        {104, "cái lều"},
        {105, "quần áo"},
        {106, "năm"},
        {107, "lá cờ"},
        {108, "hoa hồng"},
        {109, "ô cửa"},
        {110, "xe điện ngầm"},
        {111, "sân"},
        {112, "bàn phím"},
        {113, "đường ray"},
        {114, "màu xám"},
        {115, "chiếc ô"},
        {116, "áo sơ mi"},
        {117, "phô mai"},
        {118, "sân khấu"},
        {119, "nón"},
        {120, "dòng sông"},
        {121, "gian hàng"},
        {122, "sô cô la"},
        {123, "bảng"},
        {124, "dao"},
        {125, "băng ghế"},
        {126, "đi văng"},
        {127, "máy bay trực thăng"},
        {128, "laptop"},
        {129, "cái kệ"},
        {130, "bông cải xanh"},
        {131, "máy xay"},
        {132, "quán ăn"},
        {133, "mũ lưỡi trai"},
        {134, "con vịt"},
        {135, "cửa ra vào"},
        {136, "trái cây"},
        {137, "con voi"},
        {138, "bồn tiểu"},
        {139, "hải âu"},
        {140, "gấu trúc"},
        {141, "đường"},
        {142, "nước"},
        {143, "đưa đón"},
        {144, "đồ nội thất"},
        {145, "máy ảnh"},
        {146, "rau xà lách"},
        {147, "xa lộ"},
        {148, "mũ"},
        {149, "tượng đài"},
        {150, "màn"},
        {151, "xe đạp"},
        {152, "hộp"},
        {153, "màu cam"},
        {154, "lò vi sóng"},
        {155, "hàng hoá"},
        {156, "bình"},
        {157, "phòng"},
        {158, "ngân hàng"},
        {159, "mặt trời"},
        {160, "móng vuốt"},
        {161, "chậu"},
        {162, "lò sưởi"},
        {163, "lát cắt"},
        {164, "màu nâu"},
        {165, "lá"},
        {166, "rượu"},
        {167, "balo"},
        {168, "điêu khắc"},
        {169, "pug"},
        {170, "hành lang"},
        {171, "cà phê"},
        {172, "lối đi"},
        {173, "ly"},
        {174, "những quả cam"},
        {175, "máy vi tính"},
        {176, "gấu"},
        {177, "phòng tắm"},
        {178, "đèn để bàn"},
        {179, "lát gạch"},
        {180, "con dê"},
        {181, "hành lý"},
        {182, "rau quả"},
        {183, "bầu trời"},
        {184, "tạp dề"},
        {185, "áo ba lỗ"},
        {186, "máy bay"},
        {187, "xe lửa"},
        {188, "đường đua"},
        {189, "ván lướt sóng"},
        {190, "cửa sổ"},
        {191, "khung"},
        {192, "cỏ khô"},
        {193, "vòi"},
        {194, "hoa quả"},
        {195, "kính râm"},
        {196, "cỏ"},
        {197, "tàu hoả"},
        {198, "kính đeo"},
        {199, "thùng"},
        {200, "đồng hồ"},
        {201, "chín"},
        {202, "thịt"},
        {203, "thân cây"},
        {204, "mặt"},
        {205, "diều"},
        {206, "đoạn phim giới thiệu"},
        {207, "sân vận động"},
        {208, "găng tay"},
        {209, "tám"},
        {210, "tòa nhà"},
        {211, "quần short"},
        {212, "đồng phục"},
        {213, "mũ bảo hiểm"},
        {214, "sáu"},
        {215, "con thoi"},
        {216, "gạch"},
        {217, "cún yêu"},
        {218, "ca nô"},
        {219, "bò đực"},
        {220, "xe đẩy"},
        {221, "sữa"},
        {222, "vẽ tranh lên tường"},
        {223, "lọ cắm hoa"},
        {224, "cửa hàng"},
        {225, "chén đĩa"},
        {226, "đồi"},
        {227, "tàu"},
        {228, "bát"},
        {229, "phòng bếp"},
        {230, "vô tuyến"},
        {231, "sân vườn"},
        {232, "những bức ảnh"},
        {233, "trang thiết bị"},
        {234, "động cơ"},
        {235, "thành phần"},
        {236, "phương tiện giao thông"},
        {237, "đồ uống"},
        {238, "quả cam "},
        {239, "màu tía"},
        {240, "bao bì"},
        {241, "cái lọ"},
        {242, "xe tải"},
        {243, "bữa ăn tối"},
        {244, "nhà vệ sinh"},
        {245, "bức màn"},
        {246, "buồng"},
        {247, "bảng hiệu"},
        {248, "con thuyền"},
        {249, "hàng hóa"},
        {250, "con vẹt"},
        {251, "món ăn"},
        {252, "đường ống"},
        {253, "hồ bơi"},
        {254, "hộp đựng"},
        {255, "cái nĩa"},
        {256, "rau"},
        {257, "con bò"},
        {258, "dĩa "},
        {259, "cái bàn"},
        {260, "điện thoại"},
        {261, "con cừu"},
        {262, "trượt tuyết"},
        {263, "mười"},
        {264, "máy sấy khô"},
        {265, "ảnh chụp"},
        {266, "con heo"},
        {267, "chuồng"},
        {268, "quầy tính tiền"},
        {269, "đĩa ném"},
        {270, "nhà"},
        {271, "hotdog"},
        {272, "màu trắng"},
        {273, "nhà tắm"},
        {274, "áo vest"},
        {275, "đầm"},
        {276, "đại dương"},
        {277, "quả cam"},
        {278, "ga-ra"},
        {279, "quán bar"},
        {280, "hồ"},
        {281, "phòng ngủ"},
        {282, "ngoài trời"},
        {283, "người giữ"},
        {284, "đầu máy"},
        {285, "nước tiểu"},
        {286, "lớp học"},
        {287, "theo dõi"},
        {288, "toa xe"},
        {289, "mặt nạ"},
        {290, "bức tranh"},
        {291, "bánh ngọt"},
        {292, "ngăn"},
        {293, "vòi nước"},
        {294, "tủ đá"},
        {295, "trường hợp"},
        {296, "giỏ"},
        {297, "bình hoa"},
        {298, "bắp chân"},
        {299, "xe buýt"},
        {300, "cầu"},
        {301, "tường"},
        {302, "vợt"},
        {303, "bên"},
        {304, "tòa tháp"},
        {305, "mèo con"},
        {306, "cái ghế"},
        {307, "pizza"},
        {308, "cây"},
        {309, "máy tính"},
        {310, "ván trượt tuyết"},
        {311, "trạm"},
        {312, "bảy"},
        {313, "ngựa vằn"},
        {314, "bến tàu"},
        {315, "rào chắn"},
        {316, "quả táo"},
        {317, "thuốc lá"},
        {318, "ngoài"},
        {319, "kho"},
        {320, "lon"},
        {321, "gà tây"},
        {322, "thư viện"},
        {323, "một"},
        {324, "con khỉ"},
        {325, "giấy bạc"},
        {326, "màu xanh lá"},
        {327, "cái ví"},
        {328, "đường sắt"},
        {329, "hai"},
        {330, "xe ô tô"},
        {331, "nhà ở"},
        {332, "tách"},
        {333, "thuyền buồm"},
        {334, "con chó"},
        {335, "công cụ"},
        {336, "cái thìa"},
        {337, "áo choàng"},
        {338, "xe tay ga"},
        {339, "xe điện"},
        {340, "văn phòng"},
        {341, "chim bồ câu"},
        {342, "con dốc"},
        {343, "màu xanh dương"},
        {344, "thiết bị"},
        {345, "vòi hoa sen"},
        {346, "đĩa ăn"},
        {347, "ngựa rằn"},
        {348, "giường"},
        {349, "gương"},
        {350, "xe cộ"},
        {351, "cây gậy"},
        {352, "ván trượt"}
};

std::string map_number_to_label(int number) {
    // Kiểm tra xem số có tồn tại trong map không
    if (label_map.find(number) == label_map.end()) {
        throw std::runtime_error("Không tìm thấy ánh xạ cho số " + std::to_string(number));
    }

    // Trả về chữ tương ứng
    return label_map[number];
}


extern "C" JNIEXPORT void JNICALL
Java_org_pytorch_helloworld_NativeInterface_initModel(
        JNIEnv* env,
        jclass clazz,
        jstring commonModelPath,
        jstring commonClassifierModelPath,
        jstring numberModelPath,
        jstring colorModelPath) {

    const char *modelPathCStr = env->GetStringUTFChars(commonModelPath, nullptr);
    const char *classifier_modelPathCStr = env->GetStringUTFChars(commonClassifierModelPath, nullptr);
    const char *number_modelPathCStr = env->GetStringUTFChars(numberModelPath, nullptr);
    const char *color_modelPathCStr = env->GetStringUTFChars(colorModelPath, nullptr);

    std::string modelPathStr(modelPathCStr);
    std::string commonClassifier_modelPathStr(classifier_modelPathCStr);
    std::string number_modelPathStr(number_modelPathCStr);
    std::string color_modelPathStr(color_modelPathCStr);

    // Load the PyTorch model
    if (!isModelLoaded) {
        c10::optional<at::Device> device = at::Device(at::kCPU);
        model = torch::jit::_load_for_mobile(modelPathStr, device);
        commonClassifier_model = torch::jit::_load_for_mobile(commonClassifier_modelPathStr, device);
        number_student_model = torch::jit::_load_for_mobile(number_modelPathStr, device);
        color_student_model = torch::jit::_load_for_mobile(color_modelPathStr, device);
        isModelLoaded = true; // Đánh dấu model đã được load
    }
}


extern "C" JNIEXPORT jstring JNICALL
Java_org_pytorch_helloworld_NativeInterface_processNpyWithModel(
        JNIEnv* env,
        jclass clazz,
        jstring imagePath,
        jstring questionPath,
        jstring modelPath,
        jstring common_classify_modelPath,
        jstring number_modelPath,
        jstring color_modelPath) {

    // Convert jstring to std::string
    const char *imgPathCStr = env->GetStringUTFChars(imagePath, nullptr);
    const char *modelPathCStr = env->GetStringUTFChars(modelPath, nullptr);
    const char *classifier_modelPathCStr = env->GetStringUTFChars(common_classify_modelPath, nullptr);
    const char *questionPathCStr = env->GetStringUTFChars(questionPath, nullptr);
    const char *number_modelPathCStr = env->GetStringUTFChars(number_modelPath, nullptr);
    const char *color_modelPathCStr = env->GetStringUTFChars(color_modelPath, nullptr);

    std::string npyPathStr(imgPathCStr);
    std::string modelPathStr(modelPathCStr);
    std::string commonClassifier_modelPathStr(classifier_modelPathCStr);
    std::string questionPathStr(questionPathCStr);
    std::string number_modelPathStr(number_modelPathCStr);
    std::string color_modelPathStr(color_modelPathCStr);

//    BERT

    string inputFile = questionPathStr;
    string outputFile = "/data/data/org.pytorch.helloworld/files/outputFastBPE.txt";
    string codesFile = "/data/data/org.pytorch.helloworld/files/bpe.codes";
    string vocabFile = "/data/data/org.pytorch.helloworld/files/vocab.txt";
    fastBPE::applybpe(outputFile.c_str(), inputFile.c_str(), codesFile.c_str(), vocabFile.c_str());


    std::ifstream file(outputFile);

    // Đọc file dòng theo dòng
    std::string line;
    std::getline(file, line);
    std::cout << line << std::endl;

    // Đóng file
    file.close();

    std::ifstream file_in(outputFile);

    // Đọc file dòng theo dòng
    std::string org_question;
    std::getline(file_in, org_question);
    std::cout << org_question << std::endl;

    // Đóng file
    file.close();

    int USE_PADDING_TRUNCATE = true;
    std::unordered_map<std::string, int> vocab = loadVocab("/data/data/org.pytorch.helloworld/files/tokenizer.json");



    std::string tokenized_str = line;
    std::cout << tokenized_str << std::endl;

    std::vector<std::string> tokens;
    std::istringstream iss(tokenized_str);
    std::string s;
    while (std::getline(iss, s, ' ')) {
        std::cout << s << std::endl;
        tokens.push_back(s);
    }



    std::vector<int> input_ids, token_type_ids, attention_mask;

    if (USE_PADDING_TRUNCATE) {
        const int MAX_LENGTH = 20;
        input_ids = std::vector<int>(MAX_LENGTH, 1);
        token_type_ids = std::vector<int>(MAX_LENGTH, 0);
        attention_mask = std::vector<int>(MAX_LENGTH, 0);
        input_ids[0] = 0;
        token_type_ids[0] = 0;
        attention_mask[0] = 1;

        int last_element_index = 0;
        if (MAX_LENGTH < tokens.size() + 1) {
            last_element_index = MAX_LENGTH - 1;
        } else {
            last_element_index = tokens.size() +1 ;
        }

        for (int idx = 0; idx < std::min(MAX_LENGTH - 1, static_cast<int>(tokens.size())); ++idx) {
            int token_id = vocab.count(tokens[idx] + "</w>") ? vocab[tokens[idx] + "</w>"] : 3;
            token_type_ids[idx + 1] = 0;
            attention_mask[idx + 1] = 1;
            input_ids[idx + 1] = token_id;
        }
        input_ids[last_element_index] = 2;
        token_type_ids[last_element_index] = 0;
        attention_mask[last_element_index] = 1;
    } else {
        input_ids = {0};
        token_type_ids = {0};
        attention_mask = {1};

        for (const auto& token : tokens) {
            int token_id = vocab.count(token + "</w>") ? vocab[token + "</w>"] : 3;
            input_ids.push_back(token_id);
            token_type_ids.push_back(0);
            attention_mask.push_back(1);
        }
        input_ids.push_back(2);
        token_type_ids.push_back(0);
        attention_mask.push_back(1);
    }

    auto options = torch::TensorOptions().dtype(torch::kInt32);
    torch::Tensor input_ids_text_tensor = torch::from_blob(input_ids.data(), {1, 20}, options).clone();
    torch::Tensor token_type_ids_text_tensor = torch::from_blob(token_type_ids.data(), {1, 20}, options).clone();
    torch::Tensor attention_mask_text_tensor = torch::from_blob(attention_mask.data(), {1, 20}, options).clone();

    torch::Tensor combined_vec = torch::stack({input_ids_text_tensor,attention_mask_text_tensor, token_type_ids_text_tensor}, 1);

// End preprocess bert



    cv::Mat image = cv::imread(imgPathCStr);
    cv::cvtColor(image, image, cv::COLOR_BGR2RGB);

    cv::resize(image, image, cv::Size(224, 224));
    image.convertTo(image, CV_32FC3, 1.0f);
    image = image*1.0f/255.0f;
    cv::Scalar mean{0.5f, 0.5f, 0.5f};
    cv::Scalar std{0.5f, 0.5f, 0.5f};
    cv::subtract(image, mean, image);
    cv::divide(image, std, image);
    torch::Tensor img_tensor = torch::from_blob(image.data, {1, 3, 224, 224},torch::kFloat32).clone();  // Convert the image to a tensor.




    torch::Tensor common_outputTensor;
    std::vector<torch::jit::IValue> inputs;
    inputs.push_back(img_tensor);
    inputs.push_back(combined_vec);
    try {
        common_outputTensor = model.forward(inputs).toTensor();
    } catch (const c10::Error& e) {
;        std::cerr << "Error during forward pass: " << e.what() << std::endl;
    }
    Dict counting_dict = {
            {0, 323}, {1, 329}, {2, 26}, {3, 46}, {4, 106},
            {5, 214}, {6, 312}, {7, 209}, {8, 201}, {9, 263}
    };
    Dict color_dict = {
            {0, 20}, {1, 68}, {2, 79}, {3, 114}, {4, 153},
            {5, 164}, {6, 239}, {7, 272}, {8, 326}, {9, 343}
    };


    std::vector<torch::jit::IValue> inputs_classifier;
    inputs_classifier.push_back(common_outputTensor);
    int type_question = checkQuestion(org_question);
    int predicted_value;
    torch::Tensor Final_output;
    if (type_question == 1)
    {
//        # counting
        Final_output = number_student_model.forward(inputs_classifier).toTensor();
        // Argmax (find the index of the highest probability)
        int predicted_index = m_argmax(Final_output);
        // Lookup in Dictionary (equivalent to y_pred.item() in Python)
        predicted_value = counting_dict.at(predicted_index); // Use .at() for safety
    }
    else if (type_question == 2)
    {
//        # color
        Final_output = color_student_model.forward(inputs_classifier).toTensor();
        // Argmax (find the index of the highest probability)
        int predicted_index = m_argmax(Final_output);
        // Lookup in Dictionary (equivalent to y_pred.item() in Python)
        predicted_value = color_dict.at(predicted_index); // Use .at() for safety
    }
    else
    {
        //        # others
        Final_output = commonClassifier_model.forward(inputs_classifier).toTensor();
        // Argmax (find the index of the highest probability)
        int predicted_index = m_argmax(Final_output);
        predicted_value = predicted_index;
    }

    std::string label = map_number_to_label(predicted_value);

//    return env->NewStringUTF(label.c_str());
    // Kiểm tra xem label có chứa dữ liệu không
    if (label.empty()) {
        // Xử lý trường hợp label rỗng (ví dụ: trả về một chuỗi lỗi)
        return env->NewStringUTF("Error: label is empty");
    }

    jstring result = env->NewStringUTF(label.c_str());

    // Kiểm tra lỗi JNI
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe(); // In thông tin lỗi ra console
        return nullptr; // Hoặc trả về một chuỗi lỗi tùy chọn
    }

    return result; // Trả về chuỗi text thành công
}
