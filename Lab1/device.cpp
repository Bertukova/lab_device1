/**
 * @file main.cpp
 *
 * @brief A C++ programlsdemonstrating the usage of the Stream and Device classes.
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <cmath>

using namespace std;

int streamcounter; ///< Global variable to keep track of stream creation.

const int MIXER_OUTPUTS = 1;
const float POSSIBLE_ERROR = 0.01;

/**
 * @class Stream
 * @brief Represents a chemical stream with a name and mass flow.
 */
class Stream
{
private:
    double mass_flow; ///< The mass flow rate of the stream.
    string name;      ///< The name of the stream.

public:
    /**
     * @brief Constructor to create a Stream with a unique name.
     * @param s An integer used to generate a unique name for the stream.
     */
    Stream(int s){setName("s"+std::to_string(s));}

    /**
     * @brief Set the name of the stream.
     * @param s The new name for the stream.
     */
    void setName(string s){name=s;}

    /**
     * @brief Get the name of the stream.
     * @return The name of the stream.
     */
    string getName(){return name;}

    /**
     * @brief Set the mass flow rate of the stream.
     * @param m The new mass flow rate value.
     */
    void setMassFlow(double m){mass_flow=m;}

    /**
     * @brief Get the mass flow rate of the stream.
     * @return The mass flow rate of the stream.
     */
    double getMassFlow() const {return mass_flow;}

    /**
     * @brief Print information about the stream.
     */
    void print() { cout << "Stream " << getName() << " flow = " << getMassFlow() << endl; }
};

/**
 * @class Device
 * @brief Represents a device that manipulates chemical streams.
 */
class Device
{
protected:
    vector<shared_ptr<Stream>> inputs;  ///< Input streams connected to the device.
    vector<shared_ptr<Stream>> outputs; ///< Output streams produced by the device.
    int inputAmount;
    int outputAmount;
public:
    /**
     * @brief Add an input stream to the device.
     * @param s A shared pointer to the input stream.
     */
    void addInput(shared_ptr<Stream> s){
      if(inputs.size() < inputAmount) inputs.push_back(s);
      else throw"INPUT STREAM LIMIT!";
    }
    /**
     * @brief Add an output stream to the device.
     * @param s A shared pointer to the output stream.
     */
    void addOutput(shared_ptr<Stream> s){
      if(outputs.size() < outputAmount) outputs.push_back(s);
      else throw "OUTPUT STREAM LIMIT!";
      
    }
    vector<shared_ptr<Stream>> getInputs() const { return inputs; }
    vector<shared_ptr<Stream>> getOutputs() const { return outputs; }

    /**
     * @brief Update the output streams of the device (to be implemented by derived classes).
     */
    virtual void updateOutputs() = 0;
};

class Mixer: public Device
{
    private:
      int _inputs_count = 0;
    public:
      Mixer(int inputs_count): Device() {
        _inputs_count = inputs_count;
      }
      void addInput(shared_ptr<Stream> s) {
        if (inputs.size() == _inputs_count) {
          throw "Too much inputs"s;
        }
        inputs.push_back(s);
      }
      void addOutput(shared_ptr<Stream> s) {
        if (outputs.size() == MIXER_OUTPUTS) {
          throw "Too much outputs"s;
        }
        outputs.push_back(s);
      }
      void updateOutputs() override {
        double sum_mass_flow = 0;
        for (const auto& input_stream : inputs) {
          sum_mass_flow += input_stream -> getMassFlow();
        }

        if (outputs.empty()) {
          throw "Should set outputs before update"s;
        }

        double output_mass = sum_mass_flow / outputs.size(); // divide 0

        for (auto& output_stream : outputs) {
          output_stream -> setMassFlow(output_mass);
        }
      }
};

void shouldSetOutputsCorrectlyWithOneOutput() {
    streamcounter=0;
    Mixer d1 = Mixer(2);
    
    shared_ptr<Stream> s1(new Stream(++streamcounter));
    shared_ptr<Stream> s2(new Stream(++streamcounter));
    shared_ptr<Stream> s3(new Stream(++streamcounter));
    s1->setMassFlow(10.0);
    s2->setMassFlow(5.0);

    d1.addInput(s1);
    d1.addInput(s2);
    d1.addOutput(s3);

    d1.updateOutputs();

    if (abs(s3->getMassFlow()) - 15 < POSSIBLE_ERROR) {
      cout << "Mixer Test 1 passed"s << endl;
    } else {
      cout << "Mixer Test 1 failed"s << endl;
    }
}

void shouldCorrectOutputs() {
    streamcounter=0;
    Mixer d1 = Mixer(2);
    
    shared_ptr<Stream> s1(new Stream(++streamcounter));
    shared_ptr<Stream> s2(new Stream(++streamcounter));
    shared_ptr<Stream> s3(new Stream(++streamcounter));
    shared_ptr<Stream> s4(new Stream(++streamcounter));
    s1->setMassFlow(10.0);
    s2->setMassFlow(5.0);

    d1.addInput(s1);
    d1.addInput(s2);
    d1.addOutput(s3);

    try {
      d1.addOutput(s4);
    } catch (const string ex) {
      if (ex == "Too much outputs"s) {
        cout << "Mixer Test 2 passed"s << endl;

        return;
      }
    }

    cout << "Mixer Test 2 failed"s << endl;
}

void shouldCorrectInputs() {
    streamcounter=0;
    Mixer d1 = Mixer(2);
    
    shared_ptr<Stream> s1(new Stream(++streamcounter));
    shared_ptr<Stream> s2(new Stream(++streamcounter));
    shared_ptr<Stream> s3(new Stream(++streamcounter));
    shared_ptr<Stream> s4(new Stream(++streamcounter));
    s1->setMassFlow(10.0);
    s2->setMassFlow(5.0);

    d1.addInput(s1);
    d1.addInput(s2);
    d1.addOutput(s3);

    try {
      d1.addInput(s4);
    } catch (const string ex) {
      if (ex == "Too much inputs"s) {
        cout << "Mixer Test 3 passed"s << endl;

        return;
      }
    }

    cout << "Mixer Test 3 failed"s << endl;
}

class Reactor : public Device{
public:
    Reactor(bool isDoubleReactor) {
        inputAmount = 1;
        if (isDoubleReactor) outputAmount = 2;
        else outputAmount = 1;
    }
    
    void updateOutputs() override{
        double inputMass = inputs.at(0) -> getMassFlow();
            for(int i = 0; i < outputAmount; i++){
            double outputLocal = inputMass * (1.0/outputAmount);
            outputs.at(i) -> setMassFlow(outputLocal);
        }
    }
};

/**
 * @class Drobilka
 * @brief Represents a crusher device (1 input, 1 output)
 */
class Drobilka : public Device {
private:
    double crushFactor; // во сколько раз уменьшает поток (например, 2 = делим пополам)
public:
    Drobilka(double factor = 2.0) {
        inputAmount = 1;
        outputAmount = 1;
        if (factor <= 0) throw "Invalid factor"s;
        crushFactor = factor;
    }

    void updateOutputs() override {
        if (inputs.size() < 1) throw "NO INPUT STREAM!"s;
        if (outputs.size() < 1) throw "NO OUTPUT STREAM!"s;

        double inputMass = inputs.at(0)->getMassFlow();
        double outputMass = inputMass / crushFactor;

        outputs.at(0)->setMassFlow(outputMass);
    }
};


void testTooManyOutputStreams(){
    streamcounter=0;
    
    Reactor dl(false);
    
    shared_ptr<Stream> s1(new Stream(++streamcounter));
    shared_ptr<Stream> s2(new Stream(++streamcounter));
    shared_ptr<Stream> s3(new Stream(++streamcounter));
    s1->setMassFlow(10.0);
    s2->setMassFlow(5.0);
    dl.addInput(s1);
    dl.addOutput(s2);
    try{
        dl.addOutput(s3);
    } catch (const char* ex) {
         if (string(ex)  == "OUTPUT STREAM LIMIT!")
            cout << "Reactor Test 1 passed" << endl;

        return;
    }
    
     cout << "Reactor Test 1 failed" << endl;
}

void testTooManyInputStreams(){
    streamcounter = 0;
    
    Reactor dl(false);
    
    shared_ptr<Stream> s1(new Stream(++streamcounter));
    shared_ptr<Stream> s2(new Stream(++streamcounter));
    s1->setMassFlow(10.0);

    dl.addInput(s1);
    
    try {
        dl.addInput(s2); // добавляем второй поток — должно выбросить исключение
    } catch (const char* ex) { // ловим литерал строки
        if (string(ex) == "INPUT STREAM LIMIT!") {
            cout << "Reactor Test 2 passed" << endl;
            return;
        }
    }
    
    cout << "Reactor Test 2 failed" << endl;
}



void testInputEqualOutput(){
        streamcounter=0;
    
    Reactor dl(true);
    
    shared_ptr<Stream> s1(new Stream(++streamcounter));
    shared_ptr<Stream> s2(new Stream(++streamcounter));
    shared_ptr<Stream> s3(new Stream(++streamcounter));
    s1->setMassFlow(10.0);
    s2->setMassFlow(5.0);
    dl.addInput(s1);
    dl.addOutput(s2);
    dl.addOutput(s3);
    
    dl.updateOutputs();
    
    if (abs(dl.getOutputs().at(0)->getMassFlow() + 
        dl.getOutputs().at(1)->getMassFlow() - 
        dl.getInputs().at(0)->getMassFlow()) < POSSIBLE_ERROR)
    cout << "Reactor Test 3 passed" << endl;
    else
    cout << "Reactor Test 3 failed" << endl;
}

/**
 * @brief Проверяет, что дробилка корректно делит поток пополам.
 */
void testDrobilkaHalvesFlow() {
    streamcounter = 0; 

    Drobilka d(2.0); 
    auto s1 = make_shared<Stream>(++streamcounter); 
    auto s2 = make_shared<Stream>(++streamcounter); 

    s1->setMassFlow(10.0); 
    d.addInput(s1); 
    d.addOutput(s2); 
    d.updateOutputs(); 

    if (abs(s2->getMassFlow() - 5.0) < POSSIBLE_ERROR)
        cout << "Drobilka Test 1 passed (flow halved)" << endl;
    else
        cout << "Drobilka Test 1 failed" << endl;
}

/**
 * @brief Проверяем, что при попытке создать дробилку с нулевым коэффициентом возникает исключение.
 */
void testDrobilkaInvalidFactor() {
    try {
        Drobilka bad(0); 
        cout << "Drobilka Test 2 failed (no exception)" << endl; 
    } catch (const string& ex) { // 
        if (ex == "Invalid factor"s) 
            cout << "Drobilka Test 2 passed (invalid factor caught)" << endl;
        else
            cout << "Drobilka Test 2 failed (wrong exception)" << endl;
    } catch (...) { 
        cout << "Drobilka Test 2 failed (unknown exception)" << endl;
    }
}

/**
 * @brief Проверяет, что реактор с одним выходом сохраняет массу (вход = выход).
 */
void testReactorSingleOutput() {
    streamcounter = 0; 

    Reactor r(false); 
    auto s1 = make_shared<Stream>(++streamcounter);
    auto s2 = make_shared<Stream>(++streamcounter);

    s1->setMassFlow(12.0); 
    r.addInput(s1); 
    r.addOutput(s2); 
    r.updateOutputs(); 

    if (abs(s2->getMassFlow() - 12.0) < POSSIBLE_ERROR)
        cout << "Reactor Test 3 passed (output == input)" << endl;
    else
        cout << "Reactor Test 3 failed" << endl;
}

/**
 * @brief Проверяет, что реактор с двумя выходами делит поток поровну и сохраняет баланс масс.
 */
void testReactorDoubleOutputBalance() {
    streamcounter = 0; 

    Reactor r(true); 
    auto s1 = make_shared<Stream>(++streamcounter); 
    auto s2 = make_shared<Stream>(++streamcounter); 
    auto s3 = make_shared<Stream>(++streamcounter); 

    s1->setMassFlow(20.0); 
    r.addInput(s1); 
    r.addOutput(s2); 
    r.addOutput(s3); 
    r.updateOutputs(); 

    double totalOutput = s2->getMassFlow() + s3->getMassFlow(); 
    if (abs(totalOutput - s1->getMassFlow()) < POSSIBLE_ERROR)
        cout << "Reactor Test 4 passed (sum of outputs == input)" << endl;
    else
        cout << "Reactor Test 4 failed" << endl;
}

/**
 * @brief Проверяет, что миксер правильно суммирует входные потоки.
 */
void testMixerSumOfInputs() {
    streamcounter = 0; 

    Mixer m(2); 
    auto s1 = make_shared<Stream>(++streamcounter); 
    auto s2 = make_shared<Stream>(++streamcounter); 
    auto s3 = make_shared<Stream>(++streamcounter); 

    s1->setMassFlow(3.0); 
    s2->setMassFlow(7.0); 

    m.addInput(s1); 
    m.addInput(s2); 
    m.addOutput(s3); 
    m.updateOutputs(); 

    if (abs(s3->getMassFlow() - 10.0) < POSSIBLE_ERROR)
        cout << "Mixer Test 5 passed (output = sum of inputs)" << endl;
    else
        cout << "Mixer Test 5 failed" << endl;
}

void tests(){
    testInputEqualOutput();
    testTooManyOutputStreams();
    testTooManyInputStreams();
    testDrobilkaHalvesFlow();
    testDrobilkaInvalidFactor();
    testReactorSingleOutput();
    testReactorDoubleOutputBalance();
    testMixerSumOfInputs();

    shouldSetOutputsCorrectlyWithOneOutput();
    shouldCorrectOutputs();
    shouldCorrectInputs();
}

/**
 * @brief The entry point of the program.
 * @return 0 on successful execution.
 */
int main()
{
    streamcounter = 0;

    // Create streams
    shared_ptr<Stream> s1(new Stream(++streamcounter));
    shared_ptr<Stream> s2(new Stream(++streamcounter));
    shared_ptr<Stream> s3(new Stream(++streamcounter));

    // Set mass flows
    s1->setMassFlow(10.0);
    s2->setMassFlow(5.0);

    // Create a device (e.g., Mixer) and add input/output streams
    // Mixer d1;
    // d1.addInput(s1);
    // d1.addInput(s2);
    // d1.addOutput(s3);

    // Update the outputs of the device
    // d1.updateOutputs();

    // Print stream information
//    s1->print();
//    s2->print();
//    s3->print();
    tests();

    return 0;
}
