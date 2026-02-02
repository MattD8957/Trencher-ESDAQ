#include <QApplication>
#include <QWidget>
#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QDoubleValidator>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Main window
    QWidget window;
    window.setWindowTitle("Controls");
    window.resize(500, 400);

    // Toggle switch (checkbox styled later if you want)
    QCheckBox *toggle = new QCheckBox("ON/OFF");

    // Text input
    QLineEdit *input = new QLineEdit();
    QDoubleValidator *doubleValidator = new QDoubleValidator(0.0, 100.0, 3, input);
    doubleValidator->setNotation(QDoubleValidator::StandardNotation);
    doubleValidator->setLocale(QLocale::C);
    input->setValidator(doubleValidator);
    input->setPlaceholderText("Enter numeric value here");


    // Output label
    QLabel *output = new QLabel("Output will appear here");

    // Predertermined Value
    QLabel *valueBox = new QLabel("Value: 0");
    valueBox->setFrameStyle(QFrame::Box);
    valueBox->setAlignment(Qt::AlignCenter);

    QPushButton *updateButton = new QPushButton("Update Value");
    // Layout
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(toggle);
    layout->addWidget(input);
    layout->addWidget(output);
    layout->addWidget(valueBox);
    layout->addWidget(updateButton);

    window.setLayout(layout);

    // Logic: toggle output
    QObject::connect(toggle, &QCheckBox::toggled, [&](bool checked){
        output->setText(checked ? "Toggle is ON" : "Toggle is OFF");
    });

    // Logic: text input output
    QObject::connect(input, &QLineEdit::textChanged, [&](const QString &text){
        output->setText("Value: " + text);
    });

    window.show();
    return app.exec();
}
