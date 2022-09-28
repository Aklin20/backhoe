
#include <bake-val.h>


#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>


#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
#define SERVOMIN  150 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  600 // this is the 'maximum' pulse length count (out of 4096)

RF24 radio(8, 7); // CE, CS

struct SRadioData {
  int32_t counter;
  int8_t ax, ay, bx, by;
  int8_t as, bs;
  int16_t flags;
  uint16_t hlam[12];
};



// =SERVO=
enum {
  SFSTEER = 0,
  SLLOADER,
  SRLOADER,
  SRSTEER,
  SDRIVE,
  SPUSHER,
  SROUNDER,
  SHOE,
  SDEPTH,
  SANGLE,
  SBUCKET,

  STOTAL,
};

enum {
  SET_MAX = 127,
  SET_MIN = -127,
};



// =NEXT=
struct SNext {
  int indx;
  int curr;
  int next;
  int step;
  int imin;
  int imax;
  int izer;
};

#define NEXT_SLOW (5)
#define NEXT_FAST (10)
struct SNext next[STOTAL] = {
  {SFSTEER,  0, 0, NEXT_FAST, 470, 180, 324}, // ,
  {SLLOADER, 0, 0, NEXT_SLOW, 465, 180, 322}, // ,
  {SRLOADER, 0, 0, NEXT_SLOW, 185, 472, 328}, // ,
  {SRSTEER,  0, 0, NEXT_FAST, 235, 525, 380}, // ,
  {SDRIVE,   0, 0, NEXT_FAST, 347, 309, 328}, // ,
  {SPUSHER,  0, 0, NEXT_FAST, 430, 140, 325}, // ,
  {SROUNDER, 0, 0, NEXT_SLOW, 120, 560, 340}, // ,
  {SHOE,     0, 0, NEXT_FAST, 375, 170, 285}, // ,
  {SDEPTH,   0, 0, NEXT_FAST, 170, 420, 295}, // ,
  {SANGLE,   0, 0, NEXT_SLOW, 120, 500, 310}, // ,
  {SBUCKET,  0, 0, NEXT_FAST, 440, 180, 240}, // ,
};
void nextStep(int n, int power) {
  int p;
  p = map(power, SET_MIN, SET_MAX, -next[n].step, next[n].step);

  next[n].next = next[n].curr + p;
  if (next[n].next > SET_MAX)
    next[n].next = SET_MAX;
  if (next[n].next < SET_MIN)
    next[n].next = SET_MIN;

  if (next[n].curr == next[n].next)
    return;

  if (0) {
    VAL("NS(", n, 2);
    VAL(", ", power, 4);
    VAL(", ", p, 4);
    VAL(", ", next[n].next, 4);
    Serial.println(")");
  };
};

long next_mon_timer = 0;
int nextMon() {
  long m = millis();
  if (m > next_mon_timer + 10) {
    next_mon_timer = m;
  } else {
    return -1;
  };

  int i;
  int moves = 0;
  int curr;
  for (i = 0; i < STOTAL; ++i) {
    struct SNext &n = next[i];
    if (n.curr == n.next)
      continue;

    curr = n.curr;
    if (n.curr < n.next) {
      if (n.curr + n.step > n.next)
        n.curr = n.next;
      else
        n.curr = n.curr + n.step;

    } else {
      if (n.curr - n.step < n.next)
        n.curr = n.next;
      else
        n.curr = n.curr - n.step;

    };

    if (0) {
      VAL("NM(", n.indx, 2);
      VAL(", ", curr, 4);
      VAL("-->", n.curr, 4);
      VAL("-->", n.next, 4);
      Serial.println(")");
    };
    servoUpdate(n);
    ++moves;
  };

  return moves;
};


void servoUpdate(SNext &n) {

  int w, v = n.curr;
  if (v < 0) {
    w = map(v, SET_MIN, 0, n.imin, n.izer);
  } else {
    w = map(v, 0, SET_MAX, n.izer, n.imax);
  };

  pwm.setPWM(n.indx, 0, w);

  if (0) {
    VAL("S(", n.indx, 2);
    VAL(", ", v, 4);
    VAL(", ", w, 4);
    Serial.print(")");
    Serial.println();
  };
};

#define steerSet(A)








void setup() {
  Serial.begin(19200);
  Serial.println(F("BACKHOE: staring..."));
  delay(3000);

  if (1) {
    byte radio_pipes[2][6] = {"5Node", "6Node"};
    radio.begin();
    radio.setPALevel(RF24_PA_HIGH);
    radio.setChannel(105);
    radio.setDataRate(RF24_2MBPS);
    radio.setCRCLength(RF24_CRC_16);
    radio.setAutoAck(0);
    radio.setRetries(7, 7);
    radio.setPayloadSize(sizeof(SRadioData));
    //radio.powerUp();

    radio.openWritingPipe(radio_pipes[0]);
    radio.openReadingPipe(1, radio_pipes[1]);
    radio.startListening();

    Serial.begin(19200);
    Serial.print(F("PAYLOAD="));
    Serial.print(radio.getPayloadSize());
    Serial.println();
  };

  pwm.begin();
  pwm.setPWMFreq(60);  // Analog servos run at ~60 Hz updates

  next[SLLOADER].next = SET_MIN / 4 * 3;
  next[SRLOADER].next = SET_MIN / 4 * 3;
  next[SANGLE].next = SET_MAX;
  next[SPUSHER].next = SET_MAX;

  int i;
  for (i = 0; i < STOTAL; ++i) {
    next[i].curr = next[i].next;
    servoUpdate(next[i]);
  };

  delay(1000);
  Serial.println("...");
  next[SHOE].next = SET_MIN;
  for (i = 0; i < STOTAL; ++i) {
    next[i].curr = next[i].next;
    servoUpdate(next[i]);
  };

  delay(1000);
  Serial.println("....");
  next[SDEPTH].next = SET_MIN;
  next[SROUNDER].next = SET_MIN;
  next[SPUSHER].next = SET_MIN;
  for (i = 0; i < STOTAL; ++i) {
    next[i].curr = next[i].next;
    servoUpdate(next[i]);
  };

  yield();

  Serial.println(F("...done"));
}



// =SERIAL=
/*
  #define SSSET(F) ssSet(F ## Set, &msg[1])
  void ssSet(void (*f)(int), const char *m){
  if (*m == 'm'){
    (*f)(SET_MIN);
  }else if (*m == 'M'){
    (*f)(SET_MAX);
  }else{
    (*f)(atoi(m));
  };
  }

  void serRecv(const char *msg){
    if (!*msg)
      return;

    if ('0' <= msg[0] && msg[0] <= '9'){
      servoSet(msg[0]-'0', atoi(&msg[1]));

    }else if ('a' <= msg[0] && msg[0] <= 'f'){
      servoSet(msg[0]-'a'+10, atoi(&msg[1]));

    }else if (msg[0] == 'S'){
      SSSET(steer);
    }else if (msg[0] == 'L'){
      SSSET(loader);
    }else if (msg[0] == 'D'){
      SSSET(drive);
    }else if (msg[0] == 'B'){
      SSSET(bucket);
    }else if (msg[0] == 'R'){
      SSSET(rounder);
    }else if (msg[0] == 'H'){
      SSSET(hoe);
    }else if (msg[0] == 'V'){
      SSSET(depth);
    }else if (msg[0] == 'E'){
      SSSET(angle);
    }else if (msg[0] == 'P'){
      SSSET(pusher);


    }else{
      Serial.print("U(");
      Serial.print(msg);
      Serial.println(")");
    };

  };
  void serMon() {
  char buf[200];
  int ret = 0;

  do {
    ret = Serial.readBytesUntil(';', buf, 200);
    if (!ret) {
      break;
    };

    buf[ret] = '\0';
    Serial.print('>');
    Serial.print(buf);
    Serial.println('<');
    serRecv(buf);
  } while (ret);
  };
*/


// =RADIO=
long recv_timer = 0;
int recv_lost = 0;
int recv_packs = 0;

#define RECV_HC (5)
int recv_hc = 0;
SRadioData recv_hist[RECV_HC] = {{0}};

int recvAX(SRadioData *di) {
  return di->ax;
}
int recvAY(SRadioData *di) {
  return di->ay;
}
int recvBX(SRadioData *di) {
  return di->bx;
}
int recvBY(SRadioData *di) {
  return di->by;
}
int recvAS(SRadioData *di) {
  return di->as ? SET_MAX : 0;
}
int radioFilt(int (*f)(struct SRadioData *)) {
  int i, j, x;
  int a[RECV_HC];
  for (i = 0; i < RECV_HC; ++i) {
    a[i] = (*f)(&recv_hist[i]);
  };

  for (i = 0; i < RECV_HC; ++i) {
    for (j = 1; j < RECV_HC; ++j) {
      if (a[i] > a[j]) {
        x = a[i];
        a[i] = a[j];
        a[j] = x;
      };
    };
  };

  int w = (a[RECV_HC / 2 - 1] + 2 * a[RECV_HC / 2] + a[RECV_HC / 2 + 1]) / 4;

  if (0) {
    VAL("F(", recv_hc, 5);
    for (i = 0; i < RECV_HC; ++i)
      VAL(i == 0 ? " // " : ", ", a[i], 5);
    VAL(" // ", w, 5);
    Serial.println();
  };
  if (abs(w) < 12)
    return 0;
  return w;
};
int driving_mode = 0;
void radioSet(int indx, int w) {
  if (next[indx].curr == w)
    return w;

  switch (indx) {
    case SFSTEER:
    case SRSTEER:
      if (driving_mode == 0) {
        next[SFSTEER].curr = next[SFSTEER].next = w;
        next[SRSTEER].curr = next[SRSTEER].next = w;
        servoUpdate(next[SFSTEER]);
        servoUpdate(next[SRSTEER]);
      } else {
        next[SFSTEER].curr = next[SFSTEER].next = w;
        next[SRSTEER].curr = next[SRSTEER].next = 0;
        servoUpdate(next[SFSTEER]);
        servoUpdate(next[SRSTEER]);
      };
      break;
    default:
      next[indx].curr = next[indx].next = w;
      servoUpdate(next[indx]);
  };
};
void radioStep(int indx, int w) {
  if (w == 0)
    return;
  if (abs(w) < 20)
    return;
  if (next[indx].curr == w)
    return;

  switch (indx) {
    case SLLOADER:
    case SRLOADER:
      nextStep(SLLOADER, w);
      nextStep(SRLOADER, w);
      break;
    default:
      nextStep(indx, w);
  };
};

int remote_mode = 0;
void radioDo() {
  int as = radioFilt(recvAS);
  if (as > 0) {

    if (radioFilt(recvAY) < SET_MIN / 2) {
      if (remote_mode == 0) {
        Serial.println("BACKHOE mode...");
        remote_mode = 1;

        next[SPUSHER].next = SET_MAX;
        next[SLLOADER].next = SET_MIN;
        next[SRLOADER].next = SET_MIN;
        next[SBUCKET].next = 0;
        while (nextMon())
          delay(50);

        next[SHOE].next = SET_MAX;
        next[SANGLE].next = SET_MAX / 3 * 2;
        next[SDEPTH].next = 0;
        next[SROUNDER].next = 0;
        while (nextMon())
          delay(50);

        Serial.println("...done");
      }; // else ignore

    } else if (radioFilt(recvAY) > SET_MAX / 2) {
      if (remote_mode != 0) {
        Serial.print("LOADER mode...");
        remote_mode = 0;

        next[SHOE].next = SET_MIN;
        next[SANGLE].next = SET_MAX;
        next[SDEPTH].next = SET_MIN;
        next[SROUNDER].next = SET_MIN;
        while (nextMon())
          delay(50);


        next[SPUSHER].next = SET_MIN;
        next[SLLOADER].next = SET_MIN / 4 * 3;
        next[SRLOADER].next = SET_MIN / 4 * 3;
        next[SBUCKET].next = 0;
        while (nextMon())
          delay(50);

        Serial.println("...done");
      }; // else ignore

    } else if (radioFilt(recvAX) < SET_MIN / 2) {
      if (driving_mode == 0) {
        Serial.println("DRIVING MODE simple");
        driving_mode = 1;
      }; // else ignore

    } else if (radioFilt(recvAX) > SET_MAX / 2) {
      if (driving_mode != 0) {
        Serial.println("DRIVING MODE special");
        driving_mode = 0;
      }; // else ignore
    };

  };


  if (remote_mode == 0) {
    // immidiate
    radioSet(SFSTEER, -radioFilt(recvAY));
    radioSet(SDRIVE, radioFilt(recvAX));

    // delayed
    radioStep(SLLOADER, radioFilt(recvBX));
    radioStep(SBUCKET, radioFilt(recvBY));

  } else {
    // all delayed
    radioStep(SROUNDER, radioFilt(recvAY));
    radioStep(SANGLE, -radioFilt(recvAX));
    radioStep(SDEPTH, radioFilt(recvBX));
    radioStep(SHOE, radioFilt(recvBY));
  };
};
void radioMon() {
  // reciever
  if ( radio.available() > 0 ) {

    struct SRadioData di;
    di = {0};
    radio.read(&di, sizeof(SRadioData) );

    if (di.counter == 0)
      return;

    recv_packs++;
    recv_timer = millis();
    if (0) {
      VAL("R(", recv_packs, 5);
      VAL("=", di.counter, 5);
      VAL(", ax=", di.ax, 4);
      VAL(", ay=", di.ay, 4);
      VAL(", as=", di.as, 1);
      VAL(" // bx=", di.bx, 4);
      VAL(", by=", di.by, 4);
      VAL(", bs=", di.bs, 1);
      Serial.print(")");
      Serial.println();
    } else {
      Serial.print('.');
      if ((recv_packs & 0x3F) == 0)
        Serial.println(recv_packs);
    };


    // hist
    recv_hist[(recv_hc % RECV_HC)] = di;
    recv_hc++;

    // go
    if (recv_hc >= RECV_HC)
      radioDo();

  } else if (millis() - recv_timer > 2000) {
    recv_timer = millis();
    if (1) {
      Serial.print("RADIO_MON(NO DATA ");
      Serial.print(recv_lost++);
      Serial.print(")");
      Serial.println();
    };
  };


};



// =LOOP=
void loop() {
  //serMon();
  radioMon();
  nextMon();
}


