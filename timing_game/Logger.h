#pragma once
#include "SafeUnsignedArray.h"

// Luokka joka printtailee viitteenä annettuun streamiin
class Logger {
  public: 
    static void logSinglePress(Print& stream, const uint8_t num) {
      stream.println(num);
    }

    static void logResult(Print& stream, const float goalTime, const float result, const float error) {
      stream.print("\nTavoiteaika oli ");
      stream.print(goalTime);
      stream.print("s. ");
      stream.print("Sait tulokseksi ");
      stream.print(result);
      stream.print(" s, eli virheesi oli ");
      stream.print(error);
      stream.println(" sekuntia.");
    }

    static void logAverage(Print& stream, const float average) {
      stream.print("Painallusten keskiarvo oli ");
      stream.print(average);
      stream.print(" sekuntia.\n");
    }

    static void logSTD(Print& stream, const float std) {
      stream.print("Painallusten keskihajonta oli ");
      stream.print(std);
      stream.print(" sekuntia.\n\n");
    }

    static void logArr(Print& stream, const SafeUnsignedArray& intervals) {
      stream.print("[");
      for (uint8_t i = 0; i < intervals.size(); i++) {
        stream.print(intervals[i]);
        if (i == intervals.size() - 1) {
          stream.print("]\n");
        } else {
          stream.print(", ");
        }
      }
    }
};
