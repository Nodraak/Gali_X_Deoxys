WIP

TODO
    translate
    rewrite

Sorry, this one is in french.

---

# Tips For The Future

Here are some random tips for a future robot.

## Reutilisation

* Gagner du temps sur les trucs compliqués/long/chiant (CQB horrible a faire -> routage des LMD + gestion des encodeurs)
* Faire des features plus stylées/haut niveau : Lidar, strategie d'evitement, IA (BehavioralTree), comunicaton inter robot, GUI diverses (sur robot ou sur pc ; pour debug, strategie, simu, ...), ...

-> Carte d'alim + carte Qui Bouge (Base Roulante) : tjs identique


## IO

### Capteurs

* les sharp cest pourri (ca nique un analogIn a chaque fois)
* Capteurs STM VL61 mieux car I2C (chercher d'autres capteurs aussi)

### Ax12

cest cool, car bus (les Servo cest chiant : ca nique un PWM a chaque fois), malgre le fait que le bus soit vraiment pourri


## Tools

### Ecran de debug (ecran fpga de Houelle ou autre)
ca peut etre tres cool, mais les requirements evoluent vitent en fonction de l'avancée du robot. Faut une grosse etape de reflexion/conception pour faire ca bien, de telle sorte que ca soit utilisable pour debug, configurer les actionneur/capteurs, la strategie, etc.

### Github

tres important de versionner + faire des libs (simple d'utilisation, sans effet de bord, impact limité dans le uC) avec de la doc (sur les features, les effect de bords, ...)


## Can

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

* Gui de reglage des actionneurs / capteurs / pid ...
* Simulation
* Sequences via Geogebra ?? -> prometeur, mais au final bof
* remplacer les encodeurs par des souris d'ordi ou un mix une souris + autre chose (lidar, capteurs de dist, accelero, gyro, ...)
* Xbee to other robot (cur pos, goal pos, speed ?, status (eating balls, dispensing balls, blocked, ... taking cylinders, disp cylinders, ...))
* ecran fpga ? clear, print at x y, get clic
* DebugGui + sim
* MotionPlanner -> behavioural trees
    * path finder algo (a*, ...) -> dont forget about max speed
    * struct Object / Obstacle (can be static or moving, can give points or not, ...)
* jumper d'alim pour la L476RG
* Kalman cest pas ouf
* Le PID cest ouf
* Simplifiez vous la vie : ne passer votre temps a recompiler, faites une gui + xbee pour tester et recompiler uen fois a la fin pour sauvegarder les settings en ROM. => raspberry !
* git ftw
* bugs (features?) hardware
    * L432KC: Led on A7 is fuck up (serial ??) -> warning USBTX/USBRX !! -> Serial2
    * L432KC: Led on A5 fuck up PWM on D5 ?? not confirmed
    * D5 fuck up PWM on A5
    * 476RG: PC15 wtf pas de digi out ???
