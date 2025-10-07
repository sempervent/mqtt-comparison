package com.mqttcomparison;

import picocli.CommandLine;
import picocli.CommandLine.Command;

/**
 * Main entry point for Java MQTT comparison implementation.
 */
@Command(name = "mqtt-java", 
         description = "MQTT Java Implementation for comparison project",
         subcommands = {Publisher.class, Subscriber.class})
public class Main {
    public static void main(String[] args) {
        int exitCode = new CommandLine(new Main())
                .addSubcommand("publisher", new Publisher())
                .addSubcommand("subscriber", new Subscriber())
                .execute(args);
        System.exit(exitCode);
    }
}
