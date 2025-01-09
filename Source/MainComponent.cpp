#include "MainComponent.h"
#include "TrackThumbnail.h"

//==============================================================================

class MainComponent::PlayTrack {
public:
    PlayTrack(TextButton* solo, TextButton* mute, TextButton* record, DrawableButton* option, TextButton* forward, TextButton* backward, DrawableButton* open, DrawableButton* crop, Label* fileName, Label* trackNum, juce::Slider* trackVol, TrackThumbnail* thumbnail, LevelMeterComp* levelMeter, int trackNo)
        :solo(solo), mute(mute), record(record), option(option), forward(forward), backward(backward), open(open), crop(crop), fileName(fileName), trackNum(trackNum), trackVol(trackVol), thumbnail(thumbnail), levelMeter(levelMeter), trackNo(trackNo) {}
    ScopedPointer<juce::TextButton> solo;
    ScopedPointer<juce::TextButton> mute;
    ScopedPointer<juce::TextButton> record;
    ScopedPointer<juce::DrawableButton> option;
    ScopedPointer<juce::TextButton> forward;
    ScopedPointer<juce::TextButton> backward;
    ScopedPointer<juce::DrawableButton> open;
    ScopedPointer<juce::DrawableButton> crop;
    ScopedPointer<juce::Label> fileName;
    ScopedPointer<Label> trackNum;
    ScopedPointer<juce::Slider> trackVol;
    ScopedPointer<TrackThumbnail> thumbnail;
    ScopedPointer<LevelMeterComp>levelMeter;

    int trackNo;
    int isClip = 0;
    int isRecorder = 0;
    float moveUnit = 1;
};


MainComponent::MainComponent() : playButton(juce::String("play"), juce::DrawableButton::ImageOnButtonBackground),
stopButton(juce::String("stop"), juce::DrawableButton::ImageOnButtonBackground),
addButton(juce::String("add"), juce::DrawableButton::ImageOnButtonBackground),
eqButton(juce::String("eq"), juce::DrawableButton::ImageOnButtonBackground),
recordButton(juce::String("record"), juce::DrawableButton::ImageOnButtonBackground),
nextButton(juce::String("next"), juce::DrawableButton::ImageOnButtonBackground),
previousButton(juce::String("previous"), juce::DrawableButton::ImageOnButtonBackground),
renderButton(String("render"), DrawableButton::ImageOnButtonBackground)
{
    masterInterfaceInit();
    setSize(1620, 740);

    auto eq = edit.getPluginCache().createNewPlugin(te::EqualiserPlugin::xmlTypeName, {}).get();
    auto level = edit.getPluginCache().createNewPlugin(te::LevelMeterPlugin::xmlTypeName, {}).get();

    edit.getMasterTrack()->pluginList.insertPlugin(*eq, 0, nullptr);
    edit.getMasterTrack()->pluginList.insertPlugin(*level, 1, nullptr);

    setAudioChannels(2, 2);
    startTimerHz(60);
    Inputs();

    levelMeterComp = new LevelMeterComp(edit.getMasterPluginList().getPluginsOfType<te::LevelMeterPlugin>().getLast()->measurer);
    addAndMakeVisible(levelMeterComp);
    levelMeterComp->setBounds(1050, 50, 340, 10);

}

MainComponent::~MainComponent()
{
    shutdownAudio();
    engine.getTemporaryFileManager().getTempDirectory().deleteRecursively();
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();
}

void MainComponent::releaseResources()
{

}

void MainComponent::paint(juce::Graphics& g)
{

    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    juce::Rectangle<int> area(15, 100, 1585, 600);

    g.setColour(juce::Colours::darkgrey);
    g.fillRect(area);


    g.setColour(juce::Colours::black);

    g.setColour(juce::Colours::lightgrey);
    for (int i = 1; i < 5; i++) {
        g.drawLine(400, 100 + (i*120), 1600, 100 + (i * 120));
    }
    for (int i = 1; i < 30; i++) {
        g.drawLine(400 + (1200 / 30 * i), 100, 400 + (1200 / 30 * i), 700);
    }

}

void MainComponent::paintOverChildren(juce::Graphics& g) {
    g.setColour(Colours::orange);

    auto audioPosition = playSlider.getValue();
    auto drawPosition = (audioPosition / 300.f) * 1200.f + 400;
    g.drawLine(drawPosition, 100, drawPosition, 700);

    g.setColour(Colours::black);
    g.drawLine(400, 100, 400, 700);
}

void MainComponent::resized()
{
    title.setBounds(10, 5, 200, 80);
    notice1.setBounds(230, 25, 140, 20);
    moveE.setBounds(230, 55, 100, 20);
    moveButton.setBounds(340, 55, 30, 20);

    //testButton.setBounds(1580, 10, 30, 30);
    //testButton2.setBounds(1580, 50, 30, 30);
    //notice2.setBounds(1400, 10, 140, 20);
    //notice3.setBounds(1400, 40, 140, 20);

    previousButton.setBounds(400, 10, 70, 50);
    nextButton.setBounds(previousButton.getX() + 80, 10, 70, 50);
    recordButton.setBounds(previousButton.getX() + 160, 10, 70, 50);
    playButton.setBounds(previousButton.getX() + 240, 10, 70, 50);
    stopButton.setBounds(previousButton.getX() + 320, 10, 70, 50);
    addButton.setBounds(previousButton.getX() + 400, 10, 70, 50);
    eqButton.setBounds(previousButton.getX() + 480, 10, 70, 50);
    renderButton.setBounds(previousButton.getX() + 560, 10, 70, 50);
    
    volumeSlider.setBounds(1050, 10, 350, 30);
    
    playSlider.setBounds(400, 90, 1200, 10);
    timeLine.setBounds(400, 70, 1200, 20);

}

void MainComponent::timerCallback() {

    playSlider.setValue(edit.getTransport().getCurrentPosition(), dontSendNotification);
    notice1.setText(String(edit.getTransport().getCurrentPosition()) + " sec", dontSendNotification);
    
    repaint();

}

void MainComponent::addTrack(int y) {

    juce::TextButton* mute;
    addAndMakeVisible(mute = new juce::TextButton());
    mute->setButtonText("M");
    mute->addListener(this);
    mute->setColour(juce::TextButton::buttonColourId, juce::Colours::grey);
    mute->setBounds(260, 125 + (120 * y), 30, 30);

    juce::TextButton* solo;
    addAndMakeVisible(solo = new juce::TextButton());
    solo->setButtonText("S");
    solo->addListener(this);
    solo->setColour(juce::TextButton::buttonColourId, juce::Colours::grey);
    solo->setBounds(260, 160 + (120 * y), 30, 30);

    juce::TextButton* record;
    addAndMakeVisible(record = new juce::TextButton());
    record->setButtonText("R");
    record->addListener(this);
    record->setColour(juce::TextButton::buttonColourId, juce::Colours::grey);
    record->setBounds(295, 160 + (120 * y), 30, 30);

    DrawableButton* option;
    addAndMakeVisible(option = new DrawableButton("option", DrawableButton::ImageOnButtonBackground));
    di.setImage(juce::ImageFileFormat::loadFrom(juce::File("option.png")));
    option->setImages(&di, &di, &di);
    option->addListener(this);
    option->setColour(juce::TextButton::buttonColourId, juce::Colours::grey);
    option->setBounds(365, 160 + (120 * y), 30, 30);

    TextButton* forward;
    addAndMakeVisible(forward = new juce::TextButton());
    forward->setButtonText(">");
    forward->addListener(this);
    forward->setColour(juce::TextButton::buttonColourId, juce::Colours::grey);
    forward->setBounds(365, 125 + (120 * y), 30, 30);

    TextButton* backward;
    addAndMakeVisible(backward = new juce::TextButton());
    backward->setButtonText("<");
    backward->addListener(this);
    backward->setColour(juce::TextButton::buttonColourId, juce::Colours::grey);
    backward->setBounds(330, 125 + (120 * y), 30, 30);

    DrawableButton* open;
    addAndMakeVisible(open = new DrawableButton("option", DrawableButton::ImageOnButtonBackground));
    di.setImage(juce::ImageFileFormat::loadFrom(juce::File("open.png")));
    open->setImages(&di, &di, &di);
    open->addListener(this);
    open->setColour(juce::TextButton::buttonColourId, juce::Colours::grey);
    open->setBounds(295, 125 + (120 * y), 30, 30);

    DrawableButton* crop;
    addAndMakeVisible(crop = new DrawableButton("crop", DrawableButton::ImageOnButtonBackground));
    di.setImage(juce::ImageFileFormat::loadFrom(juce::File("crop.png")));
    crop->setImages(&di, &di, &di);
    crop->addListener(this);
    crop->setColour(juce::TextButton::buttonColourId, juce::Colours::grey);
    crop->setBounds(330, 160 + (120 * y), 30, 30);

    juce::Label* fileName;
    addAndMakeVisible(fileName = new juce::Label());
    fileName->setJustificationType(juce::Justification::centred);
    fileName->setColour(juce::Label::backgroundColourId, juce::Colours::black);
    fileName->setColour(juce::Label::textColourId, juce::Colours::white);
    fileName->setBounds(50, 120 + (120 * y), 200, 20);

    juce::Label* trackNum;
    addAndMakeVisible(trackNum = new juce::Label());
    trackNum->setJustificationType(juce::Justification::centred);
    trackNum->setColour(juce::Label::backgroundColourId, juce::Colours::black);
    trackNum->setColour(juce::Label::textColourId, juce::Colours::white);
    trackNum->setBounds(20, 120 + (120 * y), 20, 20);

    juce::Slider* trackVol;
    addAndMakeVisible(trackVol = new juce::Slider());
    trackVol->setRange(0.0f, 100.0f, 1.0f);
    trackVol->setValue(100.0f);
    trackVol->addListener(this);
    trackVol->setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    trackVol->setBounds(20, 150 + (120 * y), 230, 20);

    TrackThumbnail* thumbnail;
    addAndMakeVisible(thumbnail = new TrackThumbnail());
    thumbnail->setBounds(400, 100 + (120 * y), 1200, 120);

    LevelMeterComp* levelMeter;
    addAndMakeVisible(levelMeter = new LevelMeterComp(te::getAudioTracks(edit)[y]->getLevelMeterPlugin()->measurer));
    levelMeter->setBounds(20, 180 + (120 * y), 230, 7);

    playTracks.add(new PlayTrack(solo, mute, record, option, forward, backward, open, crop, fileName, trackNum, trackVol, thumbnail, levelMeter, y));

    auto track = te::getAudioTracks(edit)[y];

    trackNum->setText(String(track->getAudioTrackNumber()), dontSendNotification);
    
    fileName->setText(track->getName(), dontSendNotification);

    auto eq = edit.getPluginCache().createNewPlugin(te::EqualiserPlugin::xmlTypeName, {}).get();
    track->pluginList.insertPlugin(*eq, 2, nullptr);
}

void MainComponent::masterInterfaceInit() {
    title.setText("Simply DAW", dontSendNotification);
    title.setJustificationType(Justification::centred);
    title.setColour(juce::Label::backgroundColourId, juce::Colours::transparentWhite);
    title.setColour(juce::Label::textColourId, juce::Colours::darkorange);
    title.setFont(juce::Font(32.0f, juce::Font::bold));
    addAndMakeVisible(&title);

    testButton.setButtonText("test");
    testButton.onClick = [this] { testButtonClicked(); };
    testButton.setColour(juce::TextButton::buttonColourId, juce::Colours::grey);
    addAndMakeVisible(&testButton);

    testButton2.setButtonText("t2");
    testButton2.onClick = [this] { testButton2Clicked(); };
    testButton2.setColour(juce::TextButton::buttonColourId, juce::Colours::grey);
    addAndMakeVisible(&testButton2);

    di.setImage(juce::ImageFileFormat::loadFrom(juce::File("render.png")));
    renderButton.setImages(&di, &di, &di);
    renderButton.onClick = [this] { renderButtonClicked(); };
    renderButton.setColour(juce::TextButton::buttonColourId, juce::Colours::grey);
    addAndMakeVisible(&renderButton);

    di.setImage(juce::ImageFileFormat::loadFrom(juce::File("previous.png")));
    previousButton.setImages(&di, &di, &di);
    previousButton.onClick = [this] { previousButtonClicked(); };
    previousButton.setColour(juce::TextButton::buttonColourId, juce::Colours::grey);
    addAndMakeVisible(&previousButton);

    di.setImage(juce::ImageFileFormat::loadFrom(juce::File("next.png")));
    nextButton.setImages(&di, &di, &di);
    nextButton.onClick = [this] { nextButtonClicked(); };
    nextButton.setColour(juce::TextButton::buttonColourId, juce::Colours::grey);
    addAndMakeVisible(&nextButton);

    di.setImage(juce::ImageFileFormat::loadFrom(juce::File("play.png")));
    playButton.setImages(&di, &di, &di);
    playButton.onClick = [this] { playButtonClicked(); };
    playButton.setColour(juce::TextButton::buttonColourId, juce::Colours::grey);
    addAndMakeVisible(&playButton);

    di.setImage(juce::ImageFileFormat::loadFrom(juce::File("stop.png")));
    stopButton.setImages(&di, &di, &di);
    stopButton.onClick = [this] { stopButtonClicked(); };
    stopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::grey);
    addAndMakeVisible(&stopButton);

    di.setImage(juce::ImageFileFormat::loadFrom(juce::File("add.png")));
    addButton.setImages(&di, &di, &di);
    addButton.onClick = [this] { addButtonClicked(); };
    addButton.setColour(juce::TextButton::buttonColourId, juce::Colours::grey);
    addAndMakeVisible(&addButton);

    di.setImage(juce::ImageFileFormat::loadFrom(juce::File("eq.png")));
    eqButton.setImages(&di, &di, &di);
    eqButton.onClick = [this] { eqButtonClicked(); };
    eqButton.setColour(juce::TextButton::buttonColourId, juce::Colours::grey);
    addAndMakeVisible(&eqButton);

    di.setImage(juce::ImageFileFormat::loadFrom(juce::File("record.png")));
    recordButton.setImages(&di, &di, &di);
    recordButton.onClick = [this] { recordButtonClicked(); };
    recordButton.setColour(juce::TextButton::buttonColourId, juce::Colours::grey);
    addAndMakeVisible(&recordButton);

    volumeSlider.setRange(0.0f, 100.0f, 1.0f);
    volumeSlider.setValue(94);
    volumeSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    volumeSlider.addListener(this);
    addAndMakeVisible(&volumeSlider);

    playSlider.setRange(0.0f, 300.0f, 0.01f);
    playSlider.setValue(0);
    playSlider.setSliderStyle(juce::Slider::SliderStyle::LinearBar);
    playSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    playSlider.onValueChange = [this] {playSliderChanged(); };
    addAndMakeVisible(&playSlider);

    addAndMakeVisible(&timeLine);

    moveE.setInputRestrictions(6, "0123456789.");
    moveE.setText("0");
    addAndMakeVisible(&moveE);

    moveButton.setButtonText(">");
    moveButton.onClick = [this] { moveButtonClicked(); };
    moveButton.setColour(juce::TextButton::buttonColourId, juce::Colours::grey);
    addAndMakeVisible(&moveButton);


    notice1.setJustificationType(Justification::centred);
    notice1.setColour(juce::Label::backgroundColourId, juce::Colours::black);
    notice1.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(&notice1);

    notice2.setJustificationType(Justification::centred);
    notice2.setColour(juce::Label::backgroundColourId, juce::Colours::black);
    notice2.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(&notice2);

    notice3.setJustificationType(Justification::centred);
    notice3.setColour(juce::Label::backgroundColourId, juce::Colours::black);
    notice3.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(&notice3);

    addTrack(0);

    //addAndMakeVisible(a);
    //a.setBounds(330, 220, 1000, 120);
}

void MainComponent::sliderValueChanged(juce::Slider* slider) {
    if (slider == &volumeSlider) {
        edit.getMasterVolumePlugin()->setVolumeDb(slider->getValue() - 94);
        //edit.getMasterPluginList()[0]->getAutomatableParameter(1)->setParameter(slider->getValue() * )
    }
    else {
        for (const auto& i : playTracks) {
            if (slider == i->trackVol) {
                auto track = te::getAudioTracks(edit)[i->trackNo];
                track->getVolumePlugin()->setVolumeDb(slider->getValue() - 94);
            }
        }
    }
}

void MainComponent::buttonClicked(juce::Button* button) {
    for (const auto& i : playTracks) {
        if (button == i->mute) {
            muteTrack(i->trackNo);
        }
        else if (button == i->solo) {
            soloTrack(i->trackNo);
        }
        else if (button == i->option) {
            optionTrack(i->trackNo);
        }
        else if (button == i->forward) {
            forwardTrack(i->trackNo);
        }
        else if (button == i->backward) {
            backwardTrack(i->trackNo);
        }
        else if(button == i->record) {
            recordTrack(i->trackNo);
        }
        else if (button == i->open) {
            openTrack(i->trackNo);
        }
        else if (button == i->crop) {
            cropMenu(i->trackNo);
        }
    }
}

void MainComponent::playSliderChanged() {
    edit.getTransport().setCurrentPosition(playSlider.getValue());
}

void MainComponent::testButtonClicked() {
    playTracks[0]->thumbnail->setCrop(te::getAudioTracks(edit)[0]);
}

void MainComponent::testButton2Clicked() {
    
}

void MainComponent::renderButtonClicked() {
    if (!edit.getTransport().isRecording()) {
        renderingAudio();
    }
}

void MainComponent::playButtonClicked() {
    recordButton.setEnabled(false);

    if (edit.getTransport().isPlaying()) {
        edit.getTransport().setCurrentPosition(0);
        edit.getTransport().play(false);
    }
    else {
        edit.getTransport().play(false);

        di.setImage(juce::ImageFileFormat::loadFrom(juce::File("play_w.png")));
        playButton.setImages(&di, &di, &di);
        playButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
    }

    /*playButton.setButtonText("Playing");
    stopButton.setButtonText("Stop");*/
}

void MainComponent::stopButtonClicked() {
    if (edit.getTransport().isPlaying()) {
        if (edit.getTransport().isRecording()) {
            edit.getTransport().stop(false, false, true, false);
            for (auto i : playTracks) {
                if (i->isRecorder == 1) {
                    i->thumbnail->setFile(te::getAudioTracks(edit)[i->trackNo]->getClips()[0]->getCurrentSourceFile());
                    i->thumbnail->unCrop();
                    i->thumbnail->setBounds(400 + (te::getAudioTracks(edit)[i->trackNo]->getClips()[0]->getPosition().getStart())*4, 100 + (120 * i->trackNo), playTracks[i->trackNo]->thumbnail->getLength() / 300 * 1200, 120);
                    i->thumbnail->setRecord(0);
                    i->isClip = 1;
                   
                    i->solo->setEnabled(true);
                    i->mute->setEnabled(true);
                    i->record->setEnabled(true);
                    i->open->setEnabled(true);
                    i->option->setEnabled(true);
                    i->crop->setEnabled(true);
                    i->backward->setEnabled(true);
                    i->forward->setEnabled(true);
                }
            }
            di.setImage(juce::ImageFileFormat::loadFrom(juce::File("record.png")));
            recordButton.setImages(&di, &di, &di);
            recordButton.setColour(juce::TextButton::buttonColourId, juce::Colours::grey);

            renderButton.setEnabled(true);
            previousButton.setEnabled(true);
            nextButton.setEnabled(true);
            recordButton.setEnabled(true);
            playButton.setEnabled(true);
            addButton.setEnabled(true);
            eqButton.setEnabled(true);
        }
        else {
            edit.getTransport().stop(false, false, true, false);

            di.setImage(juce::ImageFileFormat::loadFrom(juce::File("play.png")));
            playButton.setImages(&di, &di, &di);
            playButton.setColour(juce::TextButton::buttonColourId, juce::Colours::grey);

            recordButton.setEnabled(true);
        }
    }
    
    else {
        edit.getTransport().setCurrentPosition(0);
        di.setImage(juce::ImageFileFormat::loadFrom(juce::File("stop.png")));
        stopButton.setImages(&di, &di, &di);
    }

}

void MainComponent::addButtonClicked() {
    if (te::getAudioTracks(edit).size() < 5) {
        int size = te::getAudioTracks(edit).size();
        edit.insertNewAudioTrack(te::TrackInsertPoint(nullptr, te::getTopLevelTracks(edit).getLast()), nullptr);
        addTrack(size);
    }    
}

void MainComponent::recordButtonClicked() {
    for (auto i : playTracks) {
        if (i->isRecorder == 1)
        {
            for (int j = 0; j < te::getAudioTracks(edit)[i->trackNo]->getClips().size(); j++) {
                te::getAudioTracks(edit)[i->trackNo]->getClips()[j]->removeFromParentTrack();

            }
            i->thumbnail->removeSource();
            i->thumbnail->setRecord(1);
            i->thumbnail->setBounds(400, 100 + (120 * i->trackNo), 1200, 120);
            i->thumbnail->repaint();

            i->solo->setEnabled(false);
            i->mute->setEnabled(false);
            i->record->setEnabled(false);
            i->open->setEnabled(false);
            i->option->setEnabled(false);
            i->crop->setEnabled(false);
            i->backward->setEnabled(false);
            i->forward->setEnabled(false);

            di.setImage(juce::ImageFileFormat::loadFrom(juce::File("record_w.png")));
            recordButton.setImages(&di, &di, &di);
            recordButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);

            renderButton.setEnabled(false);
            previousButton.setEnabled(false);
            nextButton.setEnabled(false);
            recordButton.setEnabled(false);
            playButton.setEnabled(false);
            addButton.setEnabled(false);
            eqButton.setEnabled(false);
        }
    }

    edit.getTransport().record(false, false);
}

void MainComponent::nextButtonClicked() {
    if(!edit.getTransport().isRecording()) 
        edit.getTransport().setCurrentPosition(edit.getTransport().getCurrentPosition() + 5);
}

void MainComponent::previousButtonClicked() {
    if (!edit.getTransport().isRecording())
        edit.getTransport().setCurrentPosition(edit.getTransport().getCurrentPosition() - 5);
}

void MainComponent::eqButtonClicked() { 
    eqMaster();    
}

void MainComponent::moveButtonClicked() {

    if (moveE.getText().getDoubleValue() <= 300) {
        edit.getTransport().setCurrentPosition(moveE.getText().getDoubleValue());
    }
    else {
        AlertWindow::showMessageBoxAsync(MessageBoxIconType::WarningIcon, "Unable to move!",
            "Inputed time is later than length of project",
            "OK");
    }
}

// track buttons
void MainComponent::openTrack(int trackNo) {

    if (playTracks[trackNo]->isRecorder == 0) {
        auto track = te::getAudioTracks(edit)[trackNo];
        FileChooser chooser("select a audio file...", juce::File::getSpecialLocation(juce::File::userDesktopDirectory), "*.wav");

        /*if (playTracks[trackNo]->isClip != 0) {
            track->getClips()[0]->removeFromParentTrack();
        }*/

        if (chooser.browseForFileToOpen()) {

            juce::File myFile = chooser.getResult();
            juce::String fileName = myFile.getFileName();
            double length = te::AudioFile(engine, myFile).getLength();

            track->insertWaveClip(fileName, myFile, { {0.0, length}, 0.0 }, true);

            playTracks[trackNo]->fileName->setText(fileName, juce::dontSendNotification);
            track->setName(fileName);

            playTracks[trackNo]->thumbnail->setFile(chooser.getResult());
            playTracks[trackNo]->thumbnail->setBounds(400, 100 + (120 * trackNo), playTracks[trackNo]->thumbnail->getLength() / 300 * 1200, 120);
            playTracks[trackNo]->thumbnail->unCrop();
            playTracks[trackNo]->isClip = 1;
            playTracks[trackNo]->trackVol->setValue(94);
            
        }
    }
    else {
        AlertWindow::showMessageBoxAsync(MessageBoxIconType::WarningIcon, "Can't open audio!",
            "This track is setted as record track.",
            "OK");
    }
}

void MainComponent::muteTrack(int trackNo) {

    auto track = te::getAudioTracks(edit)[trackNo];

    if (track->isMuted(true)) {
        track->setMute(false);
        playTracks[trackNo]->mute->setColour(juce::TextButton::buttonColourId, juce::Colours::grey);
    }
    else {
        track->setMute(true);
        playTracks[trackNo]->mute->setColour(juce::TextButton::buttonColourId, juce::Colours::darkcyan);
    }

}

void MainComponent::soloTrack(int trackNo) {
    auto track = te::getAudioTracks(edit)[trackNo];

    if (track->isSolo(true)) {
        track->setSolo(false);
        playTracks[trackNo]->solo->setColour(juce::TextButton::buttonColourId, juce::Colours::grey);
    }
    else {
        track->setSolo(true);
        playTracks[trackNo]->solo->setColour(juce::TextButton::buttonColourId, juce::Colours::darkgoldenrod);
    }
}

void MainComponent::removeTrack(int trackNo) {
    auto track = te::getAudioTracks(edit)[trackNo];
    edit.deleteTrack(track);

    playTracks.remove(trackNo, true);

    for (int i = trackNo; i < playTracks.size(); i++) {
        playTracks[i]->trackNo = i;
        playTracks[i]->trackNum->setText(String(te::getAudioTracks(edit)[i]->getAudioTrackNumber()), dontSendNotification);
        playTracks[i]->fileName->setText(te::getAudioTracks(edit)[i]->getName(), dontSendNotification);
       
        playTracks[i]->thumbnail->setTopLeftPosition(playTracks[i]->thumbnail->getX(), playTracks[i]->thumbnail->getY() - 120);
        playTracks[i]->solo->setTopLeftPosition(playTracks[i]->solo->getX(), playTracks[i]->solo->getY() - 120);
        playTracks[i]->mute->setTopLeftPosition(playTracks[i]->mute->getX(), playTracks[i]->mute->getY() - 120);
        playTracks[i]->record->setTopLeftPosition(playTracks[i]->record->getX(), playTracks[i]->record->getY() - 120);
        playTracks[i]->option->setTopLeftPosition(playTracks[i]->option->getX(), playTracks[i]->option->getY() - 120);
        playTracks[i]->forward->setTopLeftPosition(playTracks[i]->forward->getX(), playTracks[i]->forward->getY() - 120);
        playTracks[i]->backward->setTopLeftPosition(playTracks[i]->backward->getX(), playTracks[i]->backward->getY() - 120);
        playTracks[i]->open->setTopLeftPosition(playTracks[i]->open->getX(), playTracks[i]->open->getY() - 120);
        playTracks[i]->crop->setTopLeftPosition(playTracks[i]->crop->getX(), playTracks[i]->crop->getY() - 120);
        playTracks[i]->fileName->setTopLeftPosition(playTracks[i]->fileName->getX(), playTracks[i]->fileName->getY() - 120);
        playTracks[i]->trackNum->setTopLeftPosition(playTracks[i]->trackNum->getX(), playTracks[i]->trackNum->getY() - 120);
        playTracks[i]->trackVol->setTopLeftPosition(playTracks[i]->trackVol->getX(), playTracks[i]->trackVol->getY() - 120);
        playTracks[i]->levelMeter->setTopLeftPosition(playTracks[i]->levelMeter->getX(), playTracks[i]->levelMeter->getY() - 120);

    }

    /*for (int i = trackNo; i < te::getAudioTracks(edit).size(); i++) {
        playTracks[i]->fileName->setText(playTracks[i + 1]->fileName->getText(), dontSendNotification);
        playTracks[i]->trackVol->setValue(playTracks[i + 1]->trackVol->getValue());
        //playTracks[i]->trackNo = playTracks[i + 1]->trackNo;
        //playTracks[i]->recordTake = playTracks[i + 1]->recordTake;

        if (playTracks[i + 1]->isRecorder == 1) {
            playTracks[i]->record->setColour(juce::TextButton::buttonColourId, juce::Colours::red);
        }
        else {
            playTracks[i]->record->setColour(juce::TextButton::buttonColourId, juce::Colours::grey);
        }

        //playTracks[i]->isRecorder = playTracks[i+1]->isRecorder;
        
        if (playTracks[i + 1]->isClip == 1) {
            auto newTrack = te::getAudioTracks(edit)[i];
            playTracks[i]->thumbnail->setFile(newTrack->getClips()[0]->getCurrentSourceFile());
        }
        else {
            playTracks[i]->thumbnail->removeSource();
        }

        playTracks[i]->isClip = playTracks[i + 1]->isClip;

        auto newThumbnail = playTracks[i + 1]->thumbnail;

        playTracks[i]->thumbnail->crop(newThumbnail->getStart(), newThumbnail->getEnd());
        playTracks[i]->thumbnail->setBounds(newThumbnail->getX(), newThumbnail->getY() - 120, newThumbnail->getWidth(), newThumbnail->getHeight());
    }
    
    playTracks.removeLast();*/
}

void MainComponent::recordTrack(int trackNo) {

    if (playTracks[trackNo]->isRecorder == 0) { // 레코더 선정
        for (int i = 0; i < te::getAudioTracks(edit).size(); i++) { // 모든 트랙 레코더 해제
            playTracks[i]->isRecorder = 0;
            playTracks[i]->record->setColour(juce::TextButton::buttonColourId, juce::Colours::grey);
            for (auto instance : edit.getAllInputDevices()) {
                if (instance->getInputDevice().getDeviceType() == te::InputDevice::waveDevice) {
                    if (auto t = te::getAudioTracks(edit)[i]) {
                        instance->setTargetTrack(*t, 0, false);
                        instance->setRecordingEnabled(*t, false);
                        
                    }
                }
            }

        }

        playTracks[trackNo]->isRecorder = 1; // 해당 트랙 레코더 할당
        playTracks[trackNo]->record->setColour(juce::TextButton::buttonColourId, juce::Colours::red);
        for (auto instance : edit.getAllInputDevices()) {
            if (instance->getInputDevice().getDeviceType() == te::InputDevice::waveDevice) {
                if (auto t = te::getAudioTracks(edit)[trackNo]) {
                    instance->setTargetTrack(*t, 0, true);
                    instance->setRecordingEnabled(*t, true);
                }
            }
        }

        if (playTracks[trackNo]->isClip == 1) { // 기존에 오디오가 있을 경우 오디오 제거
            te::getAudioTracks(edit)[trackNo]->getClips()[0]->removeFromParentTrack();
            playTracks[trackNo]->thumbnail->removeSource();
            playTracks[trackNo]->thumbnail->setBounds(400, 100 + (120 * trackNo), 1200, 120);
            String name = "Track ";
            name += te::getAudioTracks(edit)[trackNo]->getAudioTrackNumber();
            te::getAudioTracks(edit)[trackNo]->setName(name);

            playTracks[trackNo]->fileName->setText(name, dontSendNotification);
            playTracks[trackNo]->isClip = 0;
        }
       
    }
    else { // 레코더 해제
        playTracks[trackNo]->isRecorder = 0;
        playTracks[trackNo]->record->setColour(juce::TextButton::buttonColourId, juce::Colours::grey);
        for (auto instance : edit.getAllInputDevices()) {
            if (instance->getInputDevice().getDeviceType() == te::InputDevice::waveDevice) {
                if (auto t = te::getAudioTracks(edit)[trackNo]) {
                    instance->setTargetTrack(*t, 0, false);
                    instance->setRecordingEnabled(*t, false);
                }
            }
        }
    }
    
}

void MainComponent::forwardTrack(int trackNo) {
    
    if (playTracks[trackNo]->isClip != 0 && te::getAudioTracks(edit)[trackNo]->getClips()[0]->getPosition().getEnd() + playTracks[trackNo]->moveUnit <= 300) {
        auto track = te::getAudioTracks(edit)[trackNo];
        auto clip = track->getClips()[0];

        clip->setStart(clip->getPosition().getStart() + playTracks[trackNo]->moveUnit, false, true);

        int position = clip->getPosition().getStart() * 4;

        playTracks[trackNo]->thumbnail->setTopLeftPosition(400 + position, playTracks[trackNo]->thumbnail->getY());
    }


}

void MainComponent::backwardTrack(int trackNo) {
    if (playTracks[trackNo]->isClip != 0 && te::getAudioTracks(edit)[trackNo]->getClips()[0]->getPosition().getStart() - playTracks[trackNo]->moveUnit >= 0) {
        auto clip = te::getAudioTracks(edit)[trackNo]->getClips()[0];

        clip->setStart(clip->getPosition().getStart() - playTracks[trackNo]->moveUnit, false, true);

        int position = clip->getPosition().getStart() * 4;

        playTracks[trackNo]->thumbnail->setTopLeftPosition(400 + position, playTracks[trackNo]->thumbnail->getY());

    }

}

void MainComponent::optionTrack(int trackNo) {
    PopupMenu menu;
    menu.addItem("Remove", [=] {removeTrack(trackNo); });

    PopupMenu moveMenu;
    moveMenu.addItem("10", [=] {selectMoveUnit(trackNo, 10); });
    moveMenu.addItem("5", [=] {selectMoveUnit(trackNo, 5); });
    moveMenu.addItem("1", [=] {selectMoveUnit(trackNo, 1); });
    moveMenu.addItem("0.5", [=] {selectMoveUnit(trackNo, 0.5); });
    moveMenu.addItem("0.25", [=] {selectMoveUnit(trackNo, 0.25); });

    menu.addSubMenu("Move", moveMenu);

    menu.addItem("Rename", [=] {renameWindowOpen(trackNo); });
    menu.addItem("Color", [=] {changeTrackColor(trackNo); });
    

    PopupMenu fx;
    
    fx.addItem("Eq", [=] {eqWindowOpen(trackNo); });
    fx.addItem("Reverb", [=] {reverbWindowOpen(trackNo); });
    fx.addItem("Delay", [=] {delayWindowOpen(trackNo); });
    fx.addItem("Compressor", [=] {compressorWindowOpen(trackNo); });

    menu.addSubMenu("Fx", fx);

    menu.showMenuAsync(PopupMenu::Options{}.withTargetComponent(playTracks[trackNo]->option));
}

void MainComponent::cropMenu(int trackNo) {
    PopupMenu cutMenu;
    cutMenu.addItem("Front", [=] {cropTrack(trackNo, 0); });
    cutMenu.addItem("Rear", [=] {cropTrack(trackNo, 1); });
    cutMenu.addItem("Range (dialog)", [=] {cropWindowOpen(trackNo); });
    cutMenu.addItem("Range (drag)", [=] {dragCrop(trackNo); });

    cutMenu.showMenuAsync(PopupMenu::Options{}.withTargetComponent(playTracks[trackNo]->crop));
}

void MainComponent::cropTrack(int trackNo, int idx) {
    if (playTracks[trackNo]->isClip != 0 && edit.getTransport().getCurrentPosition() != 0) {
       
        auto track = te::getAudioTracks(edit)[trackNo];

        track->splitAt(edit.getTransport().getCurrentPosition());

        track->getClips()[idx]->removeFromParentTrack();
        
        playTracks[trackNo]->thumbnail->crop(track->getClips()[0]->getPosition().getOffset(), track->getClips()[0]->getPosition().getOffset() + track->getClips()[0]->getPosition().getLength());

        if (idx == 0) {

            auto audioPosition = playSlider.getValue();
            auto drawPosition = (audioPosition / 300.f) * 1200.f + 400;

            playTracks[trackNo]->thumbnail->setBounds(drawPosition, 100 + (120 * trackNo), track->getClips()[0]->getPosition().getLength() / 300 * 1200, 120);
        }
        else {
            

            playTracks[trackNo]->thumbnail->setBounds(playTracks[trackNo]->thumbnail->getX(), 100 + (120 * trackNo), track->getClips()[0]->getPosition().getLength() / 300 * 1200, 120);
        }
        
    }
}

void MainComponent::cropWindowOpen(int trackNo) {
    if (playTracks[trackNo]->isClip != 0) {
        if (window)
            window->toFront(true);
        else {
            window = new CropWindow("Crop track", &edit, playTracks[trackNo]->thumbnail, trackNo);

            window->addToDesktop();

        }
    }    
}

void MainComponent::dragCrop(int trackNo) {
    if (playTracks[trackNo]->isClip != 0) {
        if (playTracks[trackNo]->thumbnail->getCrop() == 0) {
            playTracks[trackNo]->thumbnail->setCrop(te::getAudioTracks(edit)[trackNo]);
        }
    }
}

void MainComponent::changeTrackColor(int trackNo){
    auto cs = std::make_unique<ColourSelector> (ColourSelector::showAlphaChannel | ColourSelector::showColourAtTop
        | ColourSelector::editableColour
        | ColourSelector::showSliders
        | ColourSelector::showColourspace);
    
    cs->setName("test");
    cs->setCurrentColour(findColour(TextButton::buttonColourId));
    cs->addChangeListener(playTracks[trackNo]->thumbnail);
    cs->setColour(ColourSelector::backgroundColourId, Colours::transparentBlack);
    cs->setSize(300, 400);

    CallOutBox::launchAsynchronously(std::move(cs), getScreenBounds(), nullptr);
    
}

void MainComponent::renameWindowOpen(int trackNo) {
    if (renameWindow)
        renameWindow->toFront(true);
    else {
        renameWindow = new RenameWindow("Rename track", te::getAudioTracks(edit)[trackNo], playTracks[trackNo]->fileName, trackNo);

        renameWindow->addToDesktop();

    }
}

void MainComponent::selectMoveUnit(int trackNo, float unit) {
    playTracks[trackNo]->moveUnit = unit;
}

void MainComponent::Inputs() {
    auto& dm = engine.getDeviceManager();

    for (int i = 0; i < dm.getNumWaveInDevices(); i++)
        if (auto wip = dm.getWaveInDevice(i))
            wip->setStereoPair(true);

    for (int i = 0; i < dm.getNumWaveInDevices(); i++) {
        if (auto wip = dm.getWaveInDevice(i)) {
            wip->setEnabled(true);
            wip->setEndToEnd(true);
        }
    }

    edit.getTransport().ensureContextAllocated();

    edit.restartPlayback();
}

void MainComponent::eqMaster() {
    if (eqWindow)
        eqWindow->toFront(true);
    else {
        eqWindow = new EqWindow("Master Equaliser");

        eqWindow->addToDesktop();
        eqWindow->setEdit(&edit);
        
        float low = edit.getMasterTrack()->getAllPlugins()[0]->getAutomatableParameter(1)->getCurrentValue();
        float low_mid = edit.getMasterTrack()->getAllPlugins()[0]->getAutomatableParameter(10)->getCurrentValue();
        float high_mid = edit.getMasterTrack()->getAllPlugins()[0]->getAutomatableParameter(7)->getCurrentValue();
        float high = edit.getMasterTrack()->getAllPlugins()[0]->getAutomatableParameter(4)->getCurrentValue();

        eqWindow->setPreEq(low, low_mid, high_mid, high);
        
    }
}

void MainComponent::renderingAudio() {
    if (renderingWindow)
        renderingWindow->toFront(true);
    else {
        renderingWindow = new RenderingWindow("Rendering Audio", &edit);

        renderingWindow->addToDesktop();
        

    }
}

void MainComponent::eqWindowOpen(int trackNo) {

    if (eqTrackWindow)
        eqTrackWindow->toFront(true);
    else {
        eqTrackWindow = new EqTrackWindow("Track Equaliser", trackNo);

        eqTrackWindow->addToDesktop();
        eqTrackWindow->setEdit(&edit);

        float low = te::getAudioTracks(edit)[trackNo]->getAllPlugins()[2]->getAutomatableParameter(1)->getCurrentValue();
        float low_mid = te::getAudioTracks(edit)[trackNo]->getAllPlugins()[2]->getAutomatableParameter(10)->getCurrentValue();
        float high_mid = te::getAudioTracks(edit)[trackNo]->getAllPlugins()[2]->getAutomatableParameter(7)->getCurrentValue();
        float high = te::getAudioTracks(edit)[trackNo]->getAllPlugins()[2]->getAutomatableParameter(4)->getCurrentValue();

        eqTrackWindow->setPreEq(low, low_mid, high_mid, high);
    }
}

void MainComponent::reverbWindowOpen(int trackNo) {
    auto track = te::getAudioTracks(edit)[trackNo];
    int idx = track->getAllPlugins().size();

    for (int i = 3; i < track->getAllPlugins().size(); i++) {
        if (track->getAllPlugins()[i]->getPluginType() == "reverb") {
            idx = i;
        }
    }

    if (idx == track->getAllPlugins().size()) {
        auto rvb = edit.getPluginCache().createNewPlugin(te::ReverbPlugin::xmlTypeName, {}).get();
        track->pluginList.insertPlugin(rvb, idx, nullptr);
    }
    
    if (reverbWindow)
        reverbWindow->toFront(true);
    else {
        reverbWindow = new ReverbWindow("Track Reverb", track, idx);

        reverbWindow->addToDesktop();
    }
}

void MainComponent::delayWindowOpen(int trackNo) {
    auto track = te::getAudioTracks(edit)[trackNo];
    int idx = track->getAllPlugins().size();

    for (int i = 3; i < track->getAllPlugins().size(); i++) {
        if (track->getAllPlugins()[i]->getPluginType() == "delay") {
            idx = i;
        }
    }

    if (idx == track->getAllPlugins().size()) {
        auto dly = edit.getPluginCache().createNewPlugin(te::DelayPlugin::xmlTypeName, {}).get();
        track->pluginList.insertPlugin(dly, idx, nullptr);
    }

    if (delayWindow)
        delayWindow->toFront(true);
    else {
        delayWindow = new DelayWindow("Track Delay", track, idx);

        delayWindow->addToDesktop();
    }
}

void MainComponent::compressorWindowOpen(int trackNo) {
    auto track = te::getAudioTracks(edit)[trackNo];
    int idx = track->getAllPlugins().size();

    for (int i = 3; i < track->getAllPlugins().size(); i++) {
        if (track->getAllPlugins()[i]->getPluginType() == "compressor") {
            idx = i;
        }
    }

    if (idx == track->getAllPlugins().size()) {
        auto comp = edit.getPluginCache().createNewPlugin(te::CompressorPlugin::xmlTypeName, {}).get();
        track->pluginList.insertPlugin(comp, idx, nullptr);
    }

    if (compressorWindow)
        compressorWindow->toFront(true);
    else {
        compressorWindow = new CompressorWindow("Track Compressor", track, idx);

        compressorWindow->addToDesktop();
    }
}