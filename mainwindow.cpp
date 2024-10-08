#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QJSEngine>
#include <cmath>
#include <QDebug>
using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_engine(new QJSEngine(this))
{
    ui->setupUi(this);
    QJSValue mathObject = m_engine->globalObject().property("Math");
    m_engine->globalObject().setProperty("sin", mathObject.property("sin"));
    m_engine->globalObject().setProperty("cos", mathObject.property("cos"));
    m_engine->globalObject().setProperty("tan", mathObject.property("tan"));
    m_engine->globalObject().setProperty("exp", mathObject.property("exp"));
    m_engine->globalObject().setProperty("log", mathObject.property("log"));
    m_engine->globalObject().setProperty("pow", mathObject.property("pow"));
    m_engine->globalObject().setProperty("sqrt", mathObject.property("sqrt"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

double MainWindow::evaluateFunction(const QString& functionString, double x) {
    QString jsFunction = QString("(function(x) { return %1; })").arg(functionString);
    QJSValue function = m_engine->evaluate(jsFunction);

    if (function.isError()) {
        qDebug() << "Error in function:" << function.toString();
        return std::numeric_limits<double>::quiet_NaN();
    }

    QJSValueList args;
    args << x;
    QJSValue result = function.call(args);

    if (result.isError()) {
        qDebug() << "Error evaluating function:" << result.toString();
        return std::numeric_limits<double>::quiet_NaN();
    }

    return result.toNumber();
}

void MainWindow::on_pushButton_clicked()
{
    ui->plainTextEdit->clear();

    QString functionString = ui->Function->text();
    QString errorString = ui->Error->text();
    QString initAString = ui->InitA->text();
    QString initBString = ui->InitB->text();

    bool ok;
    double errorTolerance = errorString.toDouble(&ok);
    if (!ok) {
        ui->plainTextEdit->appendPlainText("Error Salah");
        return;
    }

    double a = initAString.toDouble(&ok);
    if (!ok) {
        ui->plainTextEdit->appendPlainText("Init A salah");
        return;
    }

    double b = initBString.toDouble(&ok);
    if (!ok) {
        ui->plainTextEdit->appendPlainText("Init B salah");
        return;
    }

    double fa = evaluateFunction(functionString, a);
    double fb = evaluateFunction(functionString, b);

    if (isnan(fa) || isnan(fb)) {
        ui->plainTextEdit->appendPlainText("bukan angka");
        return;
    }

    if (fa * fb >= 0) {
        ui->plainTextEdit->appendPlainText("fa * fb tidak >= 0");
        return;
    }

    double c = a;
    int iteration = 0;
    const int MAX_ITERATIONS = 1000;

    while (iteration < MAX_ITERATIONS) {
        c = (a * fb - b * fa) / (fb - fa);
        double fc = evaluateFunction(functionString, c);

        ui->plainTextEdit->appendPlainText(QString("Iteration %1: c = %2").arg(iteration).arg(c, 0, 'f', 6));

        if (fabs(fc) < errorTolerance) {
            break;
        }

        if (fc * fa < 0) {
            b = c;
            fb = fc;
        } else {
            a = c;
            fa = fc;
        }

        iteration++;
    }

    if (iteration == MAX_ITERATIONS) {
        ui->plainTextEdit->appendPlainText("Max iteration");
    } else {
        ui->plainTextEdit->appendPlainText(QString("Root found at: %1").arg(c, 0, 'f', 6));
    }
}
