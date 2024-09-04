#include "StartMenuDialog.h"

StartMenuDialog::StartMenuDialog(QWidget *parent) : QDialog(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);

    QLabel *colorLabel = new QLabel("Choose your color:", this);
    whiteButton = new QRadioButton("White", this);
    blackButton = new QRadioButton("Black", this);

    colorGroup = new QButtonGroup(this);
    colorGroup->addButton(whiteButton);
    colorGroup->addButton(blackButton);
    whiteButton->setChecked(true);

    QLabel *aiModeLabel = new QLabel("Choose AI mode:", this);
    randomAIButton = new QRadioButton("Random", this);
    oneStepAheadAIButton = new QRadioButton("One Step Ahead", this);

    aiModeGroup = new QButtonGroup(this);
    aiModeGroup->addButton(randomAIButton);
    aiModeGroup->addButton(oneStepAheadAIButton);
    randomAIButton->setChecked(true);

    okButton = new QPushButton("Start Game", this);
    connect(okButton, &QPushButton::clicked, this, &StartMenuDialog::onOkButtonClicked);

    layout->addWidget(colorLabel);
    layout->addWidget(whiteButton);
    layout->addWidget(blackButton);
    layout->addSpacing(20);
    layout->addWidget(aiModeLabel);
    layout->addWidget(randomAIButton);
    layout->addWidget(oneStepAheadAIButton);
    layout->addSpacing(20);
    layout->addWidget(okButton);

    setLayout(layout);
}

void StartMenuDialog::onOkButtonClicked() {
    bool is_white = whiteButton->isChecked();
    QString aiMode = randomAIButton->isChecked() ? "Random" : "OneStepAhead";
    emit gameSettingsSelected(is_white, aiMode);
    accept();
}
