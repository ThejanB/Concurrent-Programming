import java.util.concurrent.Semaphore;
import java.util.Random;

public class TheSleepingBarber {

    // Shared resources and synchronization primitives
    public static class BarberShop {
        Semaphore barber;
        Semaphore customer = new Semaphore(0);
        Semaphore mutex = new Semaphore(1);         // Mutex for waiting customers
        int waiting = 0;
        final int maxWaitingCustomers; 


        public BarberShop(int numberOfBarbers, int maxWaitingCustomers) {
            this.barber = new Semaphore(numberOfBarbers);
            this.maxWaitingCustomers = maxWaitingCustomers;
        }
    }

    // Customer class
    public static class Customer implements Runnable {
        private BarberShop barberShop;

        public Customer(BarberShop barberShop) {
            this.barberShop = barberShop;
        }

        @Override
        public void run() {
            try {
                barberShop.mutex.acquire();
                System.out.println("Customer " + Thread.currentThread().getId() + " arrived." + " | Waiting customers: " + barberShop.waiting + "/" + barberShop.maxWaitingCustomers);
                if (barberShop.waiting < barberShop.maxWaitingCustomers) {
                    barberShop.waiting++;
                    barberShop.mutex.release();

                    barberShop.customer.release(); // Signal barber that a customer has arrived
                    barberShop.barber.acquire();   // Wait for barber to finish haircut
                    System.out.println("Customer " + Thread.currentThread().getId() + " is getting a haircut.");

                } else {
                    barberShop.mutex.release();
                    System.out.println("Customer " + Thread.currentThread().getId() + " left (no available seats).");
                }

            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }

    // Barber class
    public static class Barber implements Runnable {
        private BarberShop barberShop;
        private int id;

        public Barber(BarberShop barberShop, int id) {
            this.barberShop = barberShop;
            this.id = id;
        }

        @Override
        public void run() {
            try {
                while (true) {
                    barberShop.customer.acquire(); // Wait for a customer to arrive
                    barberShop.mutex.acquire();
                    barberShop.waiting--;
                    barberShop.mutex.release();

                    barberShop.barber.release();   // Signal customer that barber is ready
                    System.out.println("Barber " + id + " started cutting hair.");
                    Thread.sleep(10000);            // Haircut time (mean 10 sec)
                    System.out.println("Barber " + id + " has finished cutting hair.");
                }
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }


    // Main simulation
    public static void main(String[] args) {
        double tempCustomerArrivalMean = 5;     // Customer mean inter-arrival time in sec: 5 sec
        double tempHaircutTimeMean = 10;        // Haircut mean time in sec: 10 sec
        int tempMaxWaitingCustomers = 5;     // Maximum number of waiting customers: 5 customers
        int tempNumberOfBarbers = 2;         // Number of barbers: 1 barber

        try {
            if (args.length == 2) {
                tempCustomerArrivalMean = Double.parseDouble(args[0]);
                tempHaircutTimeMean = Double.parseDouble(args[1]);
                tempMaxWaitingCustomers = Integer.parseInt(args[2]);
                tempNumberOfBarbers = Integer.parseInt(args[3]);
    
            } else if (args.length > 0) {
                System.err.println("Invalid number of input arguments. Using default values.\n");
                System.out.println("Usage: java TheSleepingBarber [customerMeanSeconds] [haircutMeanSeconds] [maxWaitingCustomers] [numberOfBarbers]");
                System.exit(0);
            }
        } catch (Exception e) {
            System.err.println("Invalid input arguments.");
            System.out.println("Usage: java TheSleepingBarber [customerMeanSeconds] [haircutMeanSeconds] [maxWaitingCustomers] [numberOfBarbers]");
            System.exit(0);
        }
        
        final double CUSTOMER_ARRIVAL_MEAN = tempCustomerArrivalMean;
        final double HAIRCUT_TIME_MEAN = tempHaircutTimeMean;
        final int MAX_WAITING_CUSTOMERS = tempMaxWaitingCustomers;
        final int NUMBER_OF_BARBERS = tempNumberOfBarbers;

        System.out.println("\nThe Sleeping Barber Problem using Semaphores starting...");
        System.out.println("Customer mean inter-arrival time: " + CUSTOMER_ARRIVAL_MEAN + " seconds");
        System.out.println("Haircut mean time: " + HAIRCUT_TIME_MEAN + " seconds");
        System.out.println("Maximum number of waiting customers: " + MAX_WAITING_CUSTOMERS + " customers");
        System.out.println("Number of barbers: " + NUMBER_OF_BARBERS + " barber(s)\n");
        
        BarberShop barberShop = new BarberShop(NUMBER_OF_BARBERS, MAX_WAITING_CUSTOMERS);
        
        // customer generator thread with 5 sec mean inter-arrival time
        Thread customerGenerator = new Thread(() -> {
            Random rand = new Random();
            while (true) {
                // Generate customer inter-arrival time (mean 5 sec)
                double lambda = CUSTOMER_ARRIVAL_MEAN*1000; // mean in milliseconds | 5 sec mean for customer arrival
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

                // Create and start a new customer
                new Thread(new Customer(barberShop)).start();
            }
        });

        // barber threads with 10 sec mean haircut time
        for (int i = 1; i <= NUMBER_OF_BARBERS; i++) {
            new Thread(new Barber(barberShop, i)).start();
        }

        customerGenerator.start(); // Start customer generator thread
        
    }
}
