
# Tips For The Future

Here are some random tips for a future robot.


## Reuse

* Save time on things that are hard / time consuming / not appealing to do (ex: CQB)
* Spend time on high level features that are more interesting or score more points (ex: advanced IA)

In particular, Power board and CQB: always the same, so why not save time and reuse them?


## IO

### Sensor

* Sharps are shit:
    * Con: Expensive
    * Con: Hard to wire a lot of them (AnalogIn is scarce)
    * Pro: they are easy to integrate (not CMS)
* STM's VL53 are better:
    * Pro: I2C bus: same number of wire for 1 or 42 sensors
    * Con: they are CMS

### Actuators

* Cheap servo:
    * Con: need one PWM for each
* Ax12:
    * Pro: UART-like proprietary bus
    * Con: UART-like proprietary bus


## Tools

### Debug Interface

* Remote debug via Xbee
* Local debug with screen

can be used as a configuration interface

* Gui de reglage des actionneurs / capteurs / pid ...

TODO

ca peut etre tres cool, mais les requirements evoluent vitent en fonction de l'avancée du robot. Faut une grosse etape de reflexion/conception pour faire ca bien, de telle sorte que ca soit utilisable pour debug, configurer les actionneur/capteurs, la strategie, etc.

### Git(hub|lab)

TOOD
tres important de versionner + faire des libs (simple d'utilisation, sans effet de bord, impact limité dans le uC) avec de la doc (sur les features, les effect de bords, ...)


## CAN bus

plutot cool. L'I2C pourrait aller.

* ca permet a n'importe quelle carte d'envoyer a n'importe quelle autre carte (ex: le lidar dit a la CQB : on est a telle coord)
* attention a pas faire de la merde :
    * 1 message = 1 carte : sinon l'arbitration ne dit rien et deux cartes envoient un payload different -> cest le crash
    * tjs avoir une carte vivante pour ACK les messages, sinon le compter augmente et ca fini en Bus off error
    * et bien gerer les prioritées
* cool pour le CV/entretiens d'embauche (true story) et se la peter


## Gali OS

* recallement (maintient de la pos a jour) -> capteur distance, lidar, (les micro switch cest assez pourri -> perte de temps + pas tjs possible, cf coupe 2017 Theme Lune)
* evitement obstacle + path finding
* strategie (behavioral tree ?)


## Miscs

* Simulation
* Sequences via Geogebra ?? -> prometeur, mais au final bof

* remplacer les encodeurs par des souris d'ordi ou un mix une souris + autre chose (lidar, capteurs de dist, accelero, gyro, ...)

* gali os / strat / ia
    * Xbee to other robot (cur pos, goal pos, speed ?, status (eating balls, dispensing balls, blocked, ... taking cylinders, disp cylinders, ...))
    * MotionPlanner -> behavioural trees
        * path finder algo (a*, ...) -> dont forget about max speed
        * struct Object / Obstacle (can be static or moving, can give points or not, ...)

    * Kalman cest pas ouf
    * Le PID cest ouf

* archi
    * 1 uc pouur MC
    * 1 rpi / linux pour strat + logs + uc flash
    * Simplifiez vous la vie : ne passer votre temps a recompiler, faites une gui + xbee pour tester et recompiler uen fois a la fin pour sauvegarder les settings en ROM. => raspberry !

* bugs (features?) hardware
    * L476 power jumper (usb vs external)
    * L432KC: Led on A7 is fuck up (serial ??) -> warning USBTX/USBRX !! -> Serial2
    * L432KC: Led on A5 fuck up PWM on D5 ?? not confirmed
    * D5 fuck up PWM on A5
    * 476RG: PC15 wtf pas de digi out ???
