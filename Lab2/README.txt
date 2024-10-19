
How ro Run ->

    1. Compilation
        - javac .\BusStopSimulation.java

    2.Run 
        2.1 Run with default values - Rider mean inter-arrival time: 30 sec, Bus mean inter-arrival time: 20 min
            - java .\BusStopSimulation.java
        2.2 Run with custom values
            - java .\BusStopSimulation.java [busMeanMinutes] [riderMeanSeconds]
                
                eg: java .\BusStopSimulation.java 25 1.5
		    java .\BusStopSimulation.java 60 20

    3. Termination
        Press Ctrl + C to terminate the simulation
        