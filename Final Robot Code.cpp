#include <FEHLCD.h>
#include <FEHMotor.h>
#include <FEHServo.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHRCS.h>
#include <FEHBattery.h>
#include <math.h>

#define CDS_Between_Value .35
#define actualPower (11.5/Battery.Voltage())
#define sebase 33.74

#define hi 41
#define lo 40

#define hi 41
#define lo 40

#define Radius 7.5

//OPTOSENSOR VALUES
#define middleVal 2.5
#define leftVal 2.2
#define rightVal 2.3


//motors
FEHMotor rightMotor(FEHMotor::Motor2,9.0);
FEHMotor leftMotor(FEHMotor::Motor1,9.0);
//encoders
DigitalEncoder leftEncoder(FEHIO::P0_0);
DigitalEncoder rightEncoder(FEHIO::P0_1);


//optosensors
AnalogInputPin leftOpto(FEHIO::P3_0);
AnalogInputPin middleOpto(FEHIO::P3_2);
AnalogInputPin rightOpto(FEHIO::P3_4);

//CdS cell
AnalogInputPin CDSCell(FEHIO::P1_0);

//Servos
FEHServo MArm(FEHServo::Servo6);
FEHServo LArm(FEHServo::Servo7);



//task functions
void humidifier();

//drive functions
void turnLeft(float);
void turnRight(float);
void goForward(float);
void goForwardT(float);
void goBackwards(float);
void goBackwardsT(float);
void turnRightIP(float);
void turnLeftIP(float);
void ramp(float);
void buttonStart();
void optosensorAlignment(float);
void doHumidifier();
void doWindow();
void backLeft(float);
void backRight(float);
void applesLever();
void apples();
void optosensorCalibration();
void startButton(float);
void compostPat();
void compostRotation();
void optoSensorAlign();
void lever();
void window();
void home();



int main(void)
{
    rightMotor.Stop();
    leftMotor.Stop();
    MArm.SetMin(500);
    MArm.SetMax(2500);

    float x, y; //for touch screen

    //Initialize the screen
    LCD.Clear(BLACK);
    LCD.SetFontColor(WHITE);

    RCS.InitializeTouchMenu("0800A4YQN");
    
    buttonStart();

    //put arms up
    LArm.SetDegree(180-95);
    MArm.SetDegree(180-95);

    //hit start button
    startButton(0.4);

    //navigate to compost bin
    backRight(135);
    goBackwardsT(1.0);
    Sleep(0.2);
    goForward(6.5);
    turnRight(90);
    Sleep(0.2);
    goBackwardsT(2.5);
    goForward(0.15);
    turnLeft(90);
    Sleep(0.3);
    goBackwards(3);

    
    compostRotation();
    

    //navigate to apple bucket 
    turnRight(90);
    goBackwardsT(1.0);
    goForward(16);
    Sleep(0.2);
    turnLeft(90);
    //optoSensorAlign();
    goBackwards(2);

    //put middle arm down and pick up bucket
    MArm.SetDegree(180-22);
    Sleep(0.75);
    goForward(3.5);
    Sleep(0.2);
    MArm.SetDegree(180-50);
    Sleep(0.1);
    MArm.SetDegree(180-90);
    Sleep(1.0);

    apples();
    humidifier();
    lever();
    window();

    return 0;
}

//optosensor code
int left(void){
    if (leftOpto.Value()>2.2){
        return 1;
    }
    else{
        return 0;
    }
}

int middle(void){
    if (middleOpto.Value()>2.5){
        return 1;
    }
    else{
        return 0;
    }
}

int right(void){
    if (rightOpto.Value()>2.2){
        return 1;
    }
    else{
        return 0;
    }
}

void optosensorAlignment(float INCHES)
{   
    int alignDist = sebase*INCHES;
    leftEncoder.ResetCounts();
    rightEncoder.ResetCounts();
    while((leftEncoder.Counts()+rightEncoder.Counts())/2<alignDist){
        if (middle()&&!left()&&!right()){
            rightMotor.SetPercent(25);
            leftMotor.SetPercent(25);
        }
        if (middleOpto.Value()<2.5){
            rightMotor.Stop();
            leftMotor.Stop();
        }

        if (left()){
            rightMotor.SetPercent(25);
            while(!middle()&&left()&&right()){
                rightMotor.Stop();
            }
        }

        if (right()){
            leftMotor.SetPercent(25);
            while(!middle()&&left()&&right()){
                leftMotor.Stop();
            }
        }

        if (right()){
            leftMotor.SetPercent(25);
            while(!middle()&&left()&&right()){
                leftMotor.Stop();
            }
        }
        if ((!middle()&&!left()&&!right())){
            break;
        }
    }
    
}

//function to press humidifier button
void doHumidifier()
{
    buttonStart();

    turnLeft(60);

    Sleep(.5);

    turnRight(60);

    Sleep(.5);

    goForward(1);

    Sleep(.5);

    turnLeft(45);

    Sleep(.5);

    ramp(6+11.5+7.5+3);

    Sleep(.5);

    turnLeft(90);

    Sleep(.5);

    goForward(8-2.5);

    Sleep(.5);

    humidifier();
    
}

void doWindow(){
    buttonStart();

    turnLeft(60);

    Sleep(.5);

    turnRight(60);

    Sleep(.5);

    goForward(1+0.5);

    Sleep(.5);

    turnLeft(45);

    Sleep(.5);

    ramp(21-0.5);

    Sleep(.5);

    turnLeft(90);

    goForwardT(0.9);

};

void turnLeft(float degrees)
{
    leftEncoder.ResetCounts();
    rightEncoder.ResetCounts();

    float radians = degrees*2*M_PI/360;

    while(rightEncoder.Counts() <= sebase*Radius*radians)
    {
        leftMotor.SetPercent(actualPower*(0));
        rightMotor.SetPercent(actualPower*(lo));
        
    }

    rightMotor.Stop();
    leftMotor.Stop();
}

void turnRight(float degrees)
{
    leftEncoder.ResetCounts();
    rightEncoder.ResetCounts();

    float radians = degrees*2*M_PI/360;

    while(leftEncoder.Counts() <= sebase*Radius*radians)
    {
        rightMotor.SetPercent(actualPower*(0.0));
        leftMotor.SetPercent(actualPower*(hi));
    }

    rightMotor.Stop();
    leftMotor.Stop();
}

void goForward(float distance)
{
    leftEncoder.ResetCounts();
    rightEncoder.ResetCounts();

    while((leftEncoder.Counts()+rightEncoder.Counts())/2 <= sebase*distance)
    {
        leftMotor.SetPercent(actualPower*(hi));
        rightMotor.SetPercent(actualPower*(lo));
        
        LCD.WriteLine("Forward");
        LCD.WriteLine(distance);
    }

    rightMotor.Stop();
    leftMotor.Stop();
}

void goForwardT(float time)
{
    leftEncoder.ResetCounts();
    rightEncoder.ResetCounts();

    leftMotor.SetPercent(actualPower*(hi));
    rightMotor.SetPercent(actualPower*(lo));
    
    Sleep(time);
    LCD.WriteLine("Counts:");
    LCD.WriteLine((leftEncoder.Counts()+rightEncoder.Counts())/2);
    

    rightMotor.Stop();
    leftMotor.Stop();
}

void goBackwards(float distance)
{
    leftEncoder.ResetCounts();
    rightEncoder.ResetCounts();

    while((leftEncoder.Counts()+rightEncoder.Counts())/2 <= sebase*distance)
    {
        rightMotor.SetPercent(actualPower*(-lo));
        leftMotor.SetPercent(actualPower*(-hi));
        LCD.WriteLine(rightEncoder.Counts());
    }

    rightMotor.Stop();
    leftMotor.Stop();
}

void goBackwardsT(float time)
{
    leftEncoder.ResetCounts();
    rightEncoder.ResetCounts();

    leftMotor.SetPercent(actualPower*(-hi));
    rightMotor.SetPercent(actualPower*(-lo));
    
    Sleep(time);
    LCD.WriteLine("Counts:");
    LCD.WriteLine((leftEncoder.Counts()+rightEncoder.Counts())/2);
    

    rightMotor.Stop();
    leftMotor.Stop();
}

void turnRightIP(float degrees)
{
    leftEncoder.ResetCounts();
    rightEncoder.ResetCounts();

    float radians = degrees*2*M_PI/360;

    leftMotor.SetPercent(actualPower*(hi));
    rightMotor.SetPercent(actualPower*(-lo));
    
    while((leftEncoder.Counts()+rightEncoder.Counts())/2 <= sebase*Radius*radians/2){
    }

    rightMotor.Stop();
    leftMotor.Stop();
}

void turnLeftIP(float degrees)
{
    leftEncoder.ResetCounts();
    rightEncoder.ResetCounts();

    float radians = degrees*2*M_PI/360;

    leftMotor.SetPercent(actualPower*(-hi));
    rightMotor.SetPercent(actualPower*(lo));
   
    while((leftEncoder.Counts()+rightEncoder.Counts())/2 <= sebase*Radius*radians/2){  
    }

    rightMotor.Stop();
    leftMotor.Stop();
}

void ramp(float distance)
{
    leftEncoder.ResetCounts();
    rightEncoder.ResetCounts();

    //go up ramp
    while(rightEncoder.Counts() <= sebase*distance)
    {
        leftMotor.SetPercent(actualPower*(hi));
        rightMotor.SetPercent(actualPower*(lo));
        
        LCD.WriteLine(rightEncoder.Counts());
    }

    rightMotor.Stop();
    leftMotor.Stop();
    Sleep(0.2);
}

void humidifier()
{
        //navigate to humidifier light
        goBackwards(2.0);
        goForwardT(0.5);
        goBackwards(7); //8.5
        MArm.SetDegree(180-90);
        Sleep(0.2);
        backLeft(90);
        goBackwardsT(1.5);
        goForward(16.0);//16.5

        float CDS_Value;

        //Code for scanning and hitting humidifier button
   
        leftEncoder.ResetCounts();
        rightEncoder.ResetCounts();

        Sleep(1.0);
   
        if (CDSCell.Value() < CDS_Between_Value)
        {
            //Color is red, hit red button
            LCD.Clear(RED);
            LCD.SetFontColor(WHITE);
            LCD.WriteLine("Color is Red");
            LCD.WriteLine(CDS_Value);
           
            turnRightIP(15);
            Sleep(0.2);
            goForward(5);
            Sleep(0.2);
            turnLeftIP(15);
            Sleep(0.2);
            goForwardT(0.15);
        }
        else
        {
            //Color is blue, hit blue button
            LCD.Clear(BLUE);
            LCD.SetFontColor(WHITE);
            LCD.WriteLine("Color is Blue");
            LCD.WriteLine(CDS_Value);
           
            turnLeftIP(15);
            Sleep(0.2);
            goForward(5);
            Sleep(0.2);
            turnRightIP(15);
            Sleep(0.2);
            goForwardT(0.15);
        }
}

void buttonStart()
{
    while(CDSCell.Value()>CDS_Between_Value);
    {
        Sleep(0.5);
    }

}

void backLeft(float degrees)
{
    leftEncoder.ResetCounts();
    rightEncoder.ResetCounts();

    float radians = degrees*2*M_PI/360;

    while(leftEncoder.Counts() <= sebase*Radius*radians)
    {
        leftMotor.SetPercent(actualPower*(-lo));
        rightMotor.SetPercent(actualPower*(0));
        
    }

    rightMotor.Stop();
    leftMotor.Stop();
}

void backRight(float degrees)
{
    leftEncoder.ResetCounts();
    rightEncoder.ResetCounts();

    float radians = degrees*2*M_PI/360;

    while(rightEncoder.Counts() <= sebase*Radius*radians)
    {
        leftMotor.SetPercent(actualPower*(0));
        rightMotor.SetPercent(actualPower*(-hi));
        
    }

    rightMotor.Stop();
    leftMotor.Stop();
}

void applesLever(){

    /*
    buttonStart();
    MArm.SetDegree(180-40);
    turnLeft(90);
    goForward(5);
    Sleep(0.2);
    turnRight(45);
    Sleep(0.2);
    turnLeft(90);
    Sleep(0.1);
    goForward(2.5);
    Sleep(0.5);
    MArm.SetDegree(180-77);
    Sleep(0.5);
    */

    backRight(180);

    Sleep(0.1);

    goForward(13);

    Sleep(0.1);

    turnLeft(88);

    goForward(23);

    turnLeft(45);

    goForward(6);

    turnRight(45);

    MArm.SetDegree(180-77-10);

    goForward(9);
    
    MArm.SetDegree(180-20);

    Sleep(0.5);

    MArm.SetDegree(180-30);

    goBackwards(6);

    turnLeft(45);

    MArm.SetDegree(180-60);

    goForward(3.5);

    turnLeft(29);

    MArm.SetDegree(180-25);

    Sleep(0.5);

    goBackwards(5);

    MArm.SetDegree(180);

    goForward(4.5);

    Sleep(0.5);

    MArm.SetDegree(180-47);

}

void optosensorCalibration(){
    while(1){
        LCD.WriteLine(leftOpto.Value());
        LCD.WriteLine(middleOpto.Value());
        LCD.WriteLine(rightOpto.Value());
        Sleep(0.2);
        LCD.Clear(BLACK);
        LCD.SetFontColor(WHITE);
    }
}

void startButton(float time)
{
    leftEncoder.ResetCounts();
    rightEncoder.ResetCounts();

    rightMotor.SetPercent(actualPower*(25));
    leftMotor.SetPercent(actualPower*(25.5));
    Sleep(time);
    LCD.WriteLine("Counts:");
    LCD.WriteLine((leftEncoder.Counts()+rightEncoder.Counts())/2);
    

    rightMotor.Stop();
    leftMotor.Stop();
}

void compostPat()
{
    //pat twice
    MArm.SetDegree(180-20);
    Sleep(0.2);
    MArm.SetDegree(180-90);
    Sleep(0.4);
    MArm.SetDegree(180-20);
    Sleep(0.2);
    MArm.SetDegree(180-90);
}

void compostRotation()
{
    //rotate compost bin
    for (int i=0; i<=2; i++){
        LArm.SetDegree(180);
        Sleep(0.2);
        goForward(2);
        Sleep(0.2);
        LArm.SetDegree(180-90);
        Sleep(0.2);
        goBackwards(2);
        Sleep(0.3);
    }
    LArm.SetDegree(180-120);
}

void apples(){

    //navigate to crate
    backRight(45);
    goBackwards(5);
    Sleep(0.2);
    backLeft(45);
    goBackwardsT(1.5);
    turnRight(90);
    goForward(27);
    Sleep(0.2);
    turnLeft(90);
    Sleep(0.2);
    goBackwardsT(1.2);
    goForward(5);
    Sleep(0.2);
    turnRight(90);
    goForwardT(0.8);
    goBackwards(0.5);
    Sleep(0.2);
    
    //put apples down
    MArm.SetDegree(180);
    Sleep(0.75);
    MArm.SetDegree(180-15);
}


//optosensor code
void optoSensorAlign()
{
    // turn until middle opto is on line
    while(middleOpto.Value()<middleVal | rightOpto.Value()<rightVal | leftOpto.Value()<leftVal)
    {
        rightMotor.SetPercent(actualPower*(lo));
        leftMotor.SetPercent(actualPower*(0));
    }
    rightMotor.Stop();
    leftMotor.Stop();

    if (leftOpto.Value()>leftVal)
    {
        while(middleOpto.Value()<middleVal)
        {
            rightMotor.SetPercent(actualPower*(0));
            leftMotor.SetPercent(actualPower*(hi));
        }
    }
    rightMotor.Stop();
    leftMotor.Stop();

    if (rightOpto.Value()>rightVal)
    {
        while(middleOpto.Value()<middleVal)
        {
            rightMotor.SetPercent(actualPower*(lo));
            leftMotor.SetPercent(actualPower*(0));
        }
    }
    rightMotor.Stop();
    leftMotor.Stop();



}

void lever()
{
    goBackwards(3);
    Sleep(0.2);
    turnRight(90);
    Sleep(0.3);
    goBackwardsT(3.0);
    goForward(13);
    Sleep(0.3);
    MArm.SetDegree(180-10); //set to just below horizontal
    Sleep(2.5);
    if (RCS.isLeverFlipped() == 1 ){//== 1 if lever is flipped
    }
    else {
        MArm.SetDegree(180-50);
        Sleep(0.5);
        turnRight(10);
    }

    Sleep(1.5);
    goBackwards(3);
    Sleep(0.2);
    MArm.SetDegree(180);
    Sleep(0.5);
    goForward(3);
    Sleep(0.3);
    MArm.SetDegree(180-30); //set to just below horizontal
    Sleep(0.5);
    MArm.SetDegree(180); //lower arm
}

void window()
{
    goBackwards(3);
    MArm.SetDegree(180-90);//put arm back up
    backLeft(90);
    goBackwardsT(4.0);
    turnLeft(85);//under-turn so we dont miss the table
    Sleep(0.2);
    goBackwardsT(2.0);
    Sleep(0.2);
    goForward(4);//left arm may run into window during turn
    Sleep(0.4);
    turnRight(85);//under-turn so we dont miss the window
    goForward(1);
    LArm.SetDegree(180-30);
    goForwardT(1.0);
}

void home()
{
    goBackwardsT(3.0);
    turnLeft(90); //possibly under-turn?
    goForwardT(4.0);
}
