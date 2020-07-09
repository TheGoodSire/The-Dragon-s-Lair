/*
  The Dragon's Lair
  Development by Spencer Jenik and Francisco Javier
  Original Artwork by Francisco Javier
  Original Game by Spencer Jenik
  
  6+ Blinks
  1+ Players
  
  Setup:
  1. Seperate out one blink per player to be treasure hunter (player) blinks and one blink to be the dragon.
  2. Triple click a blink to set to treasure hunter, long press to set blink to dragon (hold off until ready to start the game), double press to set back to lair tile.
  3. Arrange all remaining non-player (lair) blinks around the dragon in any configuration desired as long as each blink has at least one side open for connection.
     Alt Setup - If only playing with 6-9 blinks, arrange blinks in a straight line with the dragon at the end.
  4. Long press the designated dragon blink to set role and start the game.
  
  Rules:
  1. Treasures randomly spawn into the lair playspace for the players to collect.
  2. Treasure hunter (player) blinks have three colored sides - Yellow (Gold), Red (Rubies), and Green (Emeralds).
  3. Players must match the correctly colored side of their blink to a treasure to score points.
  4. Each red (ruby) and green (emerald) collected gains the player 1 point, while each yellow (gold) collected gives the player 5 points.
  5. Yellow (Gold) treasures require the play to mine (be connected to them) for 3 seconds before awarding their points.
  6. Players collect treasure points until their luck runs out (all 3 luck lost).
  7. One luck is lost by incorrectly connecting to a treausre or by being adversely affected by a dragon attack.
  8. The players current luck count is displayed by the three white lights on the treasure hunter blink.
  9. The Dragon has three attacking moves:
      A. Fireball - Orange light (Fire) moves quickly throughout the lair. Any player attached to an actively affected tile looses one luck.
      B. Poison Smoke - Pink light (Poison) moves slowly throughout the lair. Any player attached to an actively affected tile looses one luck.
      C. Void - All light is sucked from the Lair, and players must collect tiles based on their memory. The incorrect matching penalty of one health still applies. 
  10. Dragon attacks get more frequent as the game progresses eventually resulting in the end of the game.  
  
  Win Condition / End Game:
  1. After a player has lost all of their luck, their blink changes over to score mode.
  2. In score mode, lights of different colors flash in quick succession followed by a several second gap.
  3. To determine your score, count the number of Gold flashes in the group and multiply by 100, and then red by 10 and green by 1. The sum of these numbers is your score.
  4. If playing single player, try to set and beat your high score.
  5. If playing competitively with multiple players, the player with the highest score when their luck runs out wins.
  
  Resetting the Game:
  1. Double click all treasure hunter (player) blinks and all dragon blinks to temporarily set them back to lair tiles. 
  2. Set up the game again following the provided setup rules above.
  
  Challenge Mode:
  *Additional dragon blinks can be added to scale the difficulty of the game up. If playing with multiple dragons, spread them throughout the lair evenly.
  1 Dragon  - Normal
  2 Dragons - Hard Mode 
  3 Dragons - Dragon Master
 
   --------------------
   Blinks by Move38
   Brought to life via Kickstarter 2018
   @madewithblinks
   www.move38.com
   --------------------
 */

#define Reset_Interval 500 // used for animations
#define Fireball_Interval 1000 // determines length of fireball attack
#define Attack_Delay 500
#define Poison_Delay 2500 // poison uilizes three stages - This sets the length of stage 1 & 3
#define Poison_Interval 1000 // poison stage 2 length
#define Void_IntervalA 250 // void uses two stages - This sets the length of stage 1
#define Void_IntervalB 5000 // void stage 2 length
#define Mining_Interval 3000 // time it takes to mine a gold treasure

enum lairState {NN, NV, GN, GM, GP, GV, RN, RV, EN, EV, FIREBALL, POISON1, POISON2, POISON3, DRAGON, DRAGONF, DRAGONP, DRAGONV, TH, SCOREMODE, SCOREMODE2, RESET};
byte lairState = NN; //21 Unique lair states


int voidlairType = 0;
int dragonattackTime = 15000;
int hundredScore = 0; //score storage for hundreds of points
int hundredScoretemp = 0;
int tenScore = 0; //score storage for tens of points
int tenScoretemp = 0;
int Score = 0; //score storage for ones of points
int Scoretemp = 0;
int Luck = 3; //tracks current player luck

byte resetPressed = 0;

////LAIR Variables
Timer effectTimer; //multipurpose timer used to keep track of dragon attack lengths
Timer dragonTimer; //a second multipurpse timer used to keep track of things when effectTimer is already in use
Timer resetTimer; //used for animations
Timer effectdelayTimer; //timer which ensure that there is no signal blowback
Timer animation; //timer for animations

void setup() {
  // put your setup code here, to run once:
randomize(); // make sure that the treasures are unique //ensure random number generation
effectTimer.set(random(2000) + Reset_Interval); //sets initial wait for treasures to spawn
}

void resetLoop() { //Timer to reset the game
  if (effectdelayTimer.isExpired()){
        lairState = NN;  
        effectTimer.set(random(2000) + Reset_Interval);
        effectdelayTimer.set(250);
  }
}

void loop() {
  if (resetPressed == 0) {
    if (buttonLongPressed()) {
      resetPressed = 1;
      lairState = RESET;
      effectdelayTimer.set(250);
    }
    
  
    FOREACH_FACE(f) {
      if ( !isValueReceivedOnFaceExpired( f ) ) { // Have we seen an neighbor
        byte neighborGameState = getLairState(getLastValueReceivedOnFace(f));
        if (neighborGameState == RESET) {
          resetPressed = 1;
          lairState = RESET;
          effectdelayTimer.set(250);
        }
      }
    }
  } 
  if (resetPressed == 1) {
      setColor(WHITE);
  }

  
  if (buttonDoubleClicked()) { //input to set blink to treasure hunter player tile
    Score = 0;
    tenScore = 0;
    hundredScore = 0;
    lairState = TH;
  }

  if (buttonMultiClicked()) { //input to set blink to be a dragon tile
    lairState = DRAGON;
    dragonTimer.set(dragonattackTime);
  }
    
  switch (lairState) { //Detects lairState and sets loop layout
    case NN: //no treasure and no active effects
      nnLoop();
      offDisplayLoop();
      break;
    case NV: //no treasure and void effect active
      nvLoop();
      offDisplayLoop();
      break;
    case GN: //gold treasure and no active effects
      gnLoop();
      goldDisplayLoop();
      break;
    case GM: //gold treasure in mining phase
      gmLoop();
      gmDisplayLoop();
      break;
    case GP: //gold treasure in payout phase
      gpLoop();
      goldDisplayLoop();
      break;
    case GV: //gold treasure with void effect actuve
      gvLoop();
      offDisplayLoop();
      break;
    case RN: //ruby treasure with no active effects
      rnLoop();
      rubyDisplayLoop();
      break;
    case RV: //ruby treasure and void effect active
      rvLoop();
      offDisplayLoop();
      break;
    case EN: //emerald treasure with no active effects
      enLoop();
      emeraldDisplayLoop();
      break;
    case EV: //emerald treasure and void effect active
      evLoop();
      offDisplayLoop();
      break;
    case FIREBALL: //tile with active fireball effect
      fireballLoop();
      fireballDisplayLoop();
      break;
    case POISON1: //tile with active phase 1 poison effect
      poisonLoop1();
      poisonDisplayLoop();
      break;
    case POISON2: //tile with active phase 2 poison effect
      poisonLoop2();
      poisonDisplayLoop();
      break;
    case POISON3: //tile with active phase 3 poison effect
      poisonLoop3();
      poisonDisplayLoop();
      break;
    case DRAGON: //dragon tile sending no active effects
      dragonLoop();
      dragonDisplayLoop();
      break;
    case DRAGONF: //dragon tile sending fireball effect
      dragonfLoop();
      fireballDisplayLoop();
      break;
    case DRAGONP: //dragon tile sending poison effect
      dragonpLoop();
      poisonDisplayLoop();
      break;
    case DRAGONV: // dragon tile sending void effect
      dragonvLoop();
      offDisplayLoop();
      break;
    case TH: // treasure hunter (player) tile
      thLoop();
      thDisplayLoop();
      break;
    case SCOREMODE: // player tile set to score mode
      scoremodeDisplayLoop();
      break;
    case SCOREMODE2:
      scoremodeDisplayLoop2(); //necessary second phase of scoremode needed to make animation work
      break;
  }

//Tile comunication - send out active lair state
  if (effectdelayTimer.isExpired()) {
    setValueSentOnAllFaces(lairState);
  }

  if(animation.isExpired()) { //animations
    animation.set(1000+(random(5)*500));
  }
}

void nnLoop() { // Starting lairState where treasure type is generated and no dragon effects active
  voidlairType = 0;
  int newTreasure = 0;
  if (effectdelayTimer.isExpired()) {
    resetPressed = 0;
    if (effectTimer.isExpired()) { //Turning in blank lair tiles into treasure
      newTreasure = random(2);
      if (newTreasure == 0) {
        lairState = GN;
        voidlairType = 1;
      }
      if (newTreasure == 1) {
        lairState = RN;
        voidlairType = 2;
      }
      if (newTreasure == 2) {
        lairState = EN;
        voidlairType = 3;
      }
    }
  fireballpoisoncheck();
  voidcheck();
  }
}

void nvLoop() { // loop for a lair tile with no treasure and active void effect
  fireballpoisoncheck();
  if (effectTimer.isExpired()) {
    effectTimer.set(random(2000) + Reset_Interval);
    lairState = NN;
  }
} 

void gnLoop() { // loop for a lair tile with gold treasure and no active effects
  fireballpoisoncheck();
  voidcheck();
  FOREACH_FACE(f) {
    if ( !isValueReceivedOnFaceExpired( f ) ) {
      byte neighborLairState = getLairState(getLastValueReceivedOnFace(f));
      if (neighborLairState == TH) {
        effectTimer.set(Mining_Interval);
        lairState = GM;
      }
  }
 }
}

void gmLoop() { // loop for a lair tile in gold mining state
  fireballpoisoncheck();
  voidcheck();
 if (effectTimer.isExpired()) {
  effectTimer.set(250);
  lairState = GP;
 }
}

void gpLoop() { // loop for lair tile with gold treasure in a payout state
  fireballpoisoncheck();
  voidcheck();
  if (effectTimer.isExpired()) {
     effectTimer.set(random(2000) + Reset_Interval);
     lairState = NN;
  }
}

void gvLoop() { // loop for lair tile with gold treasure and void effect active
  fireballpoisoncheck();
  if (effectTimer.isExpired()) {
    lairState = GN;
  }
  FOREACH_FACE(f) {
    if ( !isValueReceivedOnFaceExpired( f ) ) {
      byte neighborLairState = getLairState(getLastValueReceivedOnFace(f));
      if (neighborLairState == TH) {
        effectTimer.set(Mining_Interval);
        lairState = GM;
      }
    }
  }
} 

void rnLoop() { // loop for lair tile with ruby treasure and no active effects
  fireballpoisoncheck();
  voidcheck();
  thcheck();
}

void rvLoop() { // loop for lair tile with ruby treasure and void effect active
  fireballpoisoncheck();
  thcheck();
  if (effectTimer.isExpired()) {
    lairState = RN;
  }
} 

void enLoop() { // loop for lair tile with emerald treasure and no dragon effects active
  fireballpoisoncheck();
  voidcheck();
  thcheck();
}

void evLoop() { // loop for lair tile with emerald treasure and no dragon effects active
  fireballpoisoncheck();
  thcheck();
  if (effectTimer.isExpired()) {
    lairState = EN;
  }
} 

void fireballLoop() { // loop for lair tiles with active fireball effect
  if (effectTimer.isExpired()) {
    effectTimer.set(random(2000) + Reset_Interval);
    effectdelayTimer.set(250);
    lairState = NN;
  }
}

void poisonLoop1() { // loop for phase one off the poison effect
  if (effectTimer.isExpired()) {
    effectTimer.set(Poison_Interval);
    lairState = POISON2;
  }
}

void poisonLoop2() { // loop for phase two of the poison effect
  if (effectTimer.isExpired()) {
    effectTimer.set(Poison_Delay);
    lairState = POISON3;
  }
}

void poisonLoop3() { // loop for phase three of the posion effect
  if (effectTimer.isExpired()) {
    effectTimer.set(random(2000) + Reset_Interval);
    lairState = NN;
  }
}

void dragonLoop() { // main dragon loop
  voidlairType = 4;
  int newLair = 3;
  if (dragonTimer.isExpired()) { // determined dragon attack type and time)
    dragonattackTime = dragonattackTime - random(1000);
    if (dragonattackTime < 1000) {
      dragonattackTime = 1000 + random(500);
    }
    dragonTimer.set(dragonattackTime);
    newLair = random(2); 
    if (newLair == 0) {
      effectTimer.set(Fireball_Interval);
      effectdelayTimer.set(Attack_Delay);
      lairState = DRAGONF;
    }
    if (newLair == 1) {
      effectTimer.set(Poison_Interval);
      effectdelayTimer.set(Attack_Delay);
      lairState = DRAGONP;
    }
    if (newLair == 2) {
      effectTimer.set(Void_IntervalA);
      
      lairState = DRAGONV;
    }
  }
}

void dragonfLoop() { // loop for when the dragon is sending out a fireball signal
  if (effectTimer.isExpired()) {
    lairState = DRAGON;
  }
}

void dragonpLoop() { // loop for when the dragon is sending out a poison signal
  if (effectTimer.isExpired()){
    lairState = DRAGON;
  }
}
  
void dragonvLoop() { // loop for when the dragon is sending out a void signal - Also used as helper loop for quasi phase one of void effect
  if (effectTimer.isExpired()) {
    effectTimer.set(Void_IntervalB);
    switch (voidlairType) {
      case 0:
      lairState = NV;
      break;
      case 1:
      lairState = GV;
      break;
      case 2:
      lairState = RV;
      break;
      case 3:
      lairState = EV;
      break;
      case 4:
      lairState = DRAGON;
      break;
    }
    
 }
} 

void thLoop() { // main treasure hunter scoring and health tracking loop
  if (effectTimer.isExpired()) { 
    if (!isValueReceivedOnFaceExpired(0)) {
      byte neighborLairState = getLairState(getLastValueReceivedOnFace(0));
      if (neighborLairState == GP) {
          Score = Score + 5;
          effectTimer.set(300);
      }
      if (neighborLairState == RN || neighborLairState == RV || neighborLairState == EN || neighborLairState == EV || neighborLairState == FIREBALL || neighborLairState == POISON1 || neighborLairState == POISON2 || neighborLairState == POISON3) {
          Luck = Luck - 1;
          effectTimer.set(1000);
       }
    }
    if (!isValueReceivedOnFaceExpired(2)) {
      byte neighborLairState = getLairState(getLastValueReceivedOnFace(2));
      if (neighborLairState == RN || neighborLairState == RV) {
        Score = Score + 1;
        effectTimer.set(300);
      }
       if (neighborLairState == GN || neighborLairState == GM || neighborLairState == GV || neighborLairState == GP || neighborLairState == EN || neighborLairState == EV || neighborLairState == FIREBALL || neighborLairState == POISON1 || neighborLairState == POISON2 || neighborLairState == POISON3) {
        Luck = Luck - 1;
        effectTimer.set(1000);
      }
    }
    if (!isValueReceivedOnFaceExpired(4)) {
      byte neighborLairState = getLairState(getLastValueReceivedOnFace(4));
      if (neighborLairState == EN || neighborLairState == EV) {
          Score = Score + 1;
          effectTimer.set(300);
      }
      if (neighborLairState == GN || neighborLairState == GM || neighborLairState == GV || neighborLairState == GP || neighborLairState == RN || neighborLairState == RV || neighborLairState == FIREBALL || neighborLairState == POISON1 || neighborLairState == POISON2 || neighborLairState == POISON3) {
          Luck = Luck - 1;
          effectTimer.set(1000);
      }
    }
  }
  if (Score > 9) {
    tenScore = tenScore + 1;
    Score = Score - 10;
  }
  if (tenScore > 9) {
    hundredScore = hundredScore + 1;
    tenScore = tenScore - 10;
  }
  if (Luck == 0) {
    hundredScoretemp = hundredScore;
    tenScoretemp = tenScore;
    Scoretemp = Score;
    dragonTimer.set(5000);
    effectTimer.set(250);
    setColor (OFF);
    lairState = SCOREMODE;
  }
}

void scoremodeDisplayLoop() { // loop used to display end game score
  if (dragonTimer.isExpired()) {
    hundredScoretemp = hundredScore;
    tenScoretemp = tenScore;
    Scoretemp = Score;
    dragonTimer.set(5000);
    effectTimer.set(250);
    setColor (OFF);
  }
  if (effectTimer.isExpired()) {
    if (hundredScoretemp > 0) {
      setColorOnFace(YELLOW, 0);
      hundredScoretemp = hundredScoretemp - 1;
    }
    if (tenScoretemp > 0) {
      setColorOnFace(RED, 2);
      tenScoretemp = tenScoretemp - 1;
    }
    if (Scoretemp > 0) {
      setColorOnFace(GREEN, 4);
      Scoretemp = Scoretemp - 1;
    }
  }
  effectTimer.set(250);
  lairState = SCOREMODE2;
}

void scoremodeDisplayLoop2() { // helper loop needed to effectively display the end game score
  if (effectTimer.isExpired()) {
    setColor(OFF);
    effectTimer.set(250);
    lairState = SCOREMODE;
  }
}

//start of the graphics section

void thDisplayLoop() { // loop used to display proper sides for matching and player luck score.
  setColorOnFace(YELLOW, 0);
  setColorOnFace(RED, 2);
  setColorOnFace(GREEN, 4);
  if (Luck == 3) {
    setColorOnFace(WHITE, 1);
    setColorOnFace(WHITE, 3);
    setColorOnFace(WHITE, 5);
  }
  if (Luck == 2) {
    setColorOnFace(WHITE, 1);
    setColorOnFace(WHITE, 3);
    setColorOnFace(OFF, 5);
  }
  if (Luck == 1) {
    setColorOnFace(WHITE, 1);
    setColorOnFace(OFF, 3);
    setColorOnFace(OFF, 5);
  }
 }   

void offDisplayLoop() { // display loop for all lair state that are dark
  setColor(OFF);
}

void goldDisplayLoop() { //display loop for non-mining gold tile
  setColor(dim(YELLOW, ((animation.getRemaining()/16)+125)));
}

void gmDisplayLoop() { //animation for gold being mined
  setColorOnFace(YELLOW, 0);
  if (effectTimer.getRemaining()>500) setColorOnFace(YELLOW, 1);
  if (effectTimer.getRemaining()>1000) setColorOnFace(YELLOW, 2);
  if (effectTimer.getRemaining()>1500) setColorOnFace(YELLOW, 3);
  if (effectTimer.getRemaining()>2000) setColorOnFace(YELLOW, 4);
  if (effectTimer.getRemaining()>2500) setColorOnFace(YELLOW, 5);
}

void rubyDisplayLoop() { //display loop for ruby treasure with no active dragon effects.
  setColor(dim(RED, animation.getRemaining()/8));
}

void emeraldDisplayLoop() { //display loop for emerald treasure with no active dragon effects.
  setColor(dim(GREEN, animation.getRemaining()/8));
}

void fireballDisplayLoop() { //display loop for lair tiles actively affected by the fireball attack
  FOREACH_FACE(f) {
    setColorOnFace(dim(ORANGE, 51 + 51*random(4)), f);
  }
}

void poisonDisplayLoop() { //display loop for lair tiles actively affected by the poison attack
  FOREACH_FACE(f) {
    setColorOnFace(dim(MAGENTA, 51 + 51*random(4)), f);
  }
}

void dragonDisplayLoop() { //dragon animation
setColorOnFace(dim(RED, animation.getRemaining()/8), 1);
setColorOnFace(dim(RED, animation.getRemaining()/8), 3);
setColorOnFace(dim(RED, animation.getRemaining()/8), 5);
setColorOnFace(RED, 0);
setColorOnFace(RED, 2);
setColorOnFace(RED, 4);
}
 
void fireballpoisoncheck() { // helper loop used to save space in main lair state loops by checking for fireball and poison effects
  FOREACH_FACE(f) {
    if ( !isValueReceivedOnFaceExpired( f ) ) {
      byte neighborLairState = getLairState(getLastValueReceivedOnFace(f));
        if (neighborLairState == FIREBALL || neighborLairState == DRAGONF) {
          effectTimer.set(Fireball_Interval);
          lairState = FIREBALL;
        }
        if (neighborLairState == POISON2 || neighborLairState == DRAGONP) {
          effectTimer.set(Poison_Delay);
          lairState = POISON1;
        }
      }    
  }
}

void voidcheck() { // helper loop used to save space in main lair state loops by checking for void effects
  FOREACH_FACE(f) {
    if ( !isValueReceivedOnFaceExpired( f ) ) {
      byte neighborLairState = getLairState(getLastValueReceivedOnFace(f));
      if (neighborLairState == DRAGONV){
        effectTimer.set(Void_IntervalA);
        lairState = DRAGONV;
      } 
  }
 }
}

void thcheck() { // helper loop used to save space in main lair state loops by checking for treasure hunter blink
   FOREACH_FACE(f) {
    if ( !isValueReceivedOnFaceExpired( f ) ) {
      byte neighborLairState = getLairState(getLastValueReceivedOnFace(f));
      if (neighborLairState == TH) {
         effectTimer.set(random(2000) + Reset_Interval);
         lairState = NN;
      }
  }
 }
}

/////////////////
//COMMUNICATION//
/////////////////

byte getLairState(byte data) { //BYTE BCDEF
  return (data & 31);
}
