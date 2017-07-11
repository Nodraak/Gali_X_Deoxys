#ifdef IAM_QENTRESORT

#ifndef COLLISIONDETECTIONSYSTEM_H_INCLUDED
#define COLLISIONDETECTIONSYSTEM_H_INCLUDED

#include "QEntreQSort/SharpSensor.h"
#include "QEntreQSort/Srf05Sensor.h"
// #include "QEntreQSort/LaserSensor.h"
#include "pinout.h"


#define SRF_COUNT               4
#define UPDATE_SRF_PERIOD       50
#define CDS_SEND_OVER_CAN       100


class CollisionDetectionSystem {
public:
    CollisionDetectionSystem(EventQueue *queue, CanMessenger *messenger) {
        sharp_[0] = new SharpSensor(SHARP1, queue);
        sharp_[1] = new SharpSensor(SHARP2, queue);
        sharp_[2] = new SharpSensor(SHARP3, queue);
        sharp_[3] = new SharpSensor(SHARP4, queue);
        sharp_[4] = new SharpSensor(SHARP5, queue);

        srf_[0] = new Srf05Sensor(SRF1_T, SRF1_E);
        srf_[1] = new Srf05Sensor(SRF2_T, SRF2_E);
        srf_[2] = new Srf05Sensor(SRF3_T, SRF3_E);
        srf_[3] = new Srf05Sensor(SRF4_T, SRF4_E);

        // register auto update -> event queue ??
        queue->call_every(UPDATE_SRF_PERIOD, callback(this, &CollisionDetectionSystem::update_srf));
        queue->call_every(CDS_SEND_OVER_CAN, callback(this, &CollisionDetectionSystem::send_CAN_frames), messenger);
    };

private:
    void update_srf(void) {
        static int i = 0;

        srf_[i]->update();
        i = (i+1)%SRF_COUNT;
    };

/*
    todo

    two CAN msg : sharp + US + misc (micro switchs + laser + ??)

    -> full interrupt / event queue
*/

    void send_CAN_frames(CanMessenger *messenger) {
        // g_debug->printf(
        //     "[CDS] sharp %4d %4d %4d %4d %4d\n",
        //     sharp_[0]->get_val(), sharp_[1]->get_val(), sharp_[2]->get_val(),
        //     sharp_[3]->get_val(), sharp_[4]->get_val()
        // );
        // g_debug->printf(
        //     "[CDS] \t\t\t\t srf %4d %4d %4d %4d\n",
        //     srf_[0]->get_val(), srf_[1]->get_val(), srf_[2]->get_val(), srf_[3]->get_val()
        // );

        messenger->send_msg_CQES_sensor_sharp_front(sharp_[0]->get_val(), sharp_[1]->get_val(), sharp_[2]->get_val());
        messenger->send_msg_CQES_sensor_sharp_back(sharp_[3]->get_val(), sharp_[4]->get_val());
        messenger->send_msg_CQES_sensor_ultrasound(srf_[0]->get_val(), srf_[1]->get_val(), srf_[2]->get_val(), srf_[3]->get_val());
    };

private:
    SharpSensor *sharp_[5];  // todo define
    Srf05Sensor *srf_[SRF_COUNT];
};

#endif // #ifndef COLLISIONDETECTIONSYSTEM_H_INCLUDED
#endif // #ifdef IAM_QENTRESORT
