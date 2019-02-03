#include <QtTest/QtTest>
#include "../src/weight.h"
#include "../src/joint.h"
#include <string>

const float EPS=0.0001;

class TestWeights: public QObject
{
    Q_OBJECT
private slots:
    void testValues();
};

void TestWeights::testValues()
{
    Joint::createFromFile("viewer/models/walk1.bvh");
    vector<Weight> VerticesWeights;
    string filename = "viewer/models/weights.txt";
    Weight::createFromFile(filename, VerticesWeights);

    QVERIFY(VerticesWeights[0].getWeight(0) == 0);
    QVERIFY(VerticesWeights[2807].getWeight(9) - 0.999999 < EPS);
    QVERIFY(VerticesWeights[2836].getWeight(10) - 0.205937 < EPS);
}

QTEST_MAIN(TestWeights)
#include "test_weights_parser.moc"
