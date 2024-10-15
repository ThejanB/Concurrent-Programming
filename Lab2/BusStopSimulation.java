//////   Bus Stop Simulation using Semaphores   //////
// Group Members:
//          200698X - Weerasekara W.M.T.B.
//          200081B - Chandeepa E.A.C.
// 2024-10-15

import java.util.concurrent.Semaphore;
import java.util.Random;

public class BusStopSimulation {

    // Shared resources and synchronization primitives
    public static class BusStop {
        Semaphore boardingAreaLock = new Semaphore(1);
        Semaphore bus = new Semaphore(0);
        Semaphore boarded = new Semaphore(0);
        Semaphore mutex = new Semaphore(1);
        int waiting = 0;
    }

    // Rider class
    public static class Rider implements Runnable {
        private BusStop busStop;

        public Rider(BusStop busStop) {
            this.busStop = busStop;
        }

        @Override
        public void run() {
            try {
                // Rider arrives at the bus stop
                busStop.boardingAreaLock.acquire(); // Enter boarding area
                busStop.mutex.acquire();
                busStop.waiting++;
                System.out.println("Rider " + Thread.currentThread().getId() + " arrived. Waiting riders: " + busStop.waiting);
                busStop.mutex.release();
                busStop.boardingAreaLock.release(); // Allow others into boarding area

                busStop.bus.acquire(); // Wait for bus to arrive
                System.out.println("Rider " + Thread.currentThread().getId() + " is boarding."); // Board the bus
                busStop.boarded.release(); // Signal that this rider has boarded

            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }

    // Bus class
    public static class Bus implements Runnable {
        private final int MAX_RIDERS = 50;   // Maximum number of riders that can board a bus
        private BusStop busStop;

        public Bus(BusStop busStop) {
            this.busStop = busStop;
        }

        @Override
        public void run() {
            try {
                // Bus arrives
                System.out.println("Bus " + Thread.currentThread().getId() + " arrived.");
                busStop.boardingAreaLock.acquire(); // Prevent new riders from entering
                busStop.mutex.acquire();
                int n = Math.min(busStop.waiting, MAX_RIDERS); // Number of riders to board | max = 50 riders
                if (n > 0) {
                    System.out.println("Bus " + Thread.currentThread().getId() + " is boarding " + n + " riders.");
                    for (int i = 0; i < n; i++) {   // Allow n rider to board
                        busStop.bus.release(); 
                    }
                    busStop.waiting -= n;
                    busStop.mutex.release();

                    for (int i = 0; i < n; i++) {  // Wait for all riders to board
                        busStop.boarded.acquire();
                    }
                } else {
                    busStop.mutex.release();
                    System.out.println("Bus " + Thread.currentThread().getId() + " is departing immediately (no riders).");
                }
                busStop.boardingAreaLock.release(); // Allow new riders into boarding area

                // Depart
                System.out.println("Bus " + Thread.currentThread().getId() + " is departing.");

            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }

    // Main simulation 
    public static void main(String[] args) {
        double tempBusArrivalMean = 20;   // Bus mean inter-arrival time in min: 20 min
        double tempRiderArrivalMean = 30; // Rider mean inter-arrival time in sec: 30 sec
        BusStop busStop = new BusStop();

        try {
            if (args.length == 2) {
                tempBusArrivalMean = Double.parseDouble(args[0]);
                tempRiderArrivalMean = Double.parseDouble(args[1]);
            } else if (args.length > 0) {
                System.err.println("Invalid number of input arguments. Using default values.\n");
                System.out.println("Usage: java BusStopSimulation [busMeanMinutes] [riderMeanSeconds]\n");
                System.exit(0);
            }
        } catch ( Exception e) {
            System.err.println("Invalid input arguments. Using default values.\n");
            System.out.println("Usage: java BusStopSimulation [busMeanMinutes] [riderMeanSeconds]\n");
            System.exit(0);
        }
        
        final double RIDER_ARRIVAL_MEAN = tempRiderArrivalMean;
        final double BUS_ARRIVAL_MEAN = tempBusArrivalMean;

        System.out.println("\nBus Stop Simulation using Semaphores starting...");
        System.out.println("Bus mean inter-arrival time: " + RIDER_ARRIVAL_MEAN + " minutes");
        System.out.println("Rider mean inter-arrival time: " + BUS_ARRIVAL_MEAN + " seconds\n");

        // Rider generator thread with 30 sec mean inter-arrival time
        Thread riderGenerator = new Thread(() -> {
            Random rand = new Random();
            while (true) {
                // Generate rider inter-arrival time (mean 30 sec)
                double lambda = RIDER_ARRIVAL_MEAN*1000; // mean in milliseconds | 30 sec mean for rider arrival
                double u = rand.nextDouble();
                if (u == 0.0) { // Prevent log(0)
                    u = 0.0001;
                }
                long interArrivalTime = (long)(-lambda * Math.log(u));

                try {
                    Thread.sleep(interArrivalTime);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }

                // Create and start a new rider
                new Thread(new Rider(busStop)).start();
            }
        });

        // Bus generator thread with 20 min mean inter-arrival time
        Thread busGenerator = new Thread(() -> {
            Random rand = new Random();
            while (true) {
                double lambda = BUS_ARRIVAL_MEAN*60*1000; // mean in milliseconds | 20 min mean for bus arrival
                double u = rand.nextDouble();
                if (u == 0.0) {  // Prevent log(0)
                    u = 0.0001;
                }
                long interArrivalTime = (long)(-lambda * Math.log(u));

                try {
                    Thread.sleep(interArrivalTime);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }

                // Create and start a new bus
                new Thread(new Bus(busStop)).start();
            }
        });

        // Start the generator threads
        riderGenerator.start();
        busGenerator.start();
    }
}




