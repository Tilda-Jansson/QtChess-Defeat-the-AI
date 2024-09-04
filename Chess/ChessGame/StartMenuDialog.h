#ifndef STARTMENUDIALOG_H
#define STARTMENUDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QButtonGroup>
#include <QLabel>

class StartMenuDialog : public QDialog {
    Q_OBJECT
public:
    explicit StartMenuDialog(QWidget *parent = nullptr);

signals:
    void gameSettingsSelected(bool is_white, QString aiMode);

private slots:
    void onOkButtonClicked();

private:
    QRadioButton *whiteButton;
    QRadioButton *blackButton;
    QButtonGroup *colorGroup;

    QRadioButton *randomAIButton;
    QRadioButton *oneStepAheadAIButton;
    QButtonGroup *aiModeGroup;

    QPushButton *okButton;
};

#endif // STARTMENUDIALOG_H
