package logger

import (
	"io"
	"log"
	"os"
)

var LoggerInstance *Logger

func init() {
	LoggerInstance = NewLogger(os.Stdout)
}

func NewLogger(StdOut io.Writer) *Logger {
	var logger = new(Logger)

	logger.STDOUT = os.Stdout
	logger.STDERR = os.Stderr
	logger.debug = false
	logger.log = log.New(StdOut, "", 0)

	return logger
}

func Info(fmt string, args ...any) {
	LoggerInstance.Info(fmt, args...)
}

func Debug(fmt string, args ...any) {
	LoggerInstance.Debug(fmt, args...)
}

func DebugError(fmt string, args ...any) {
	LoggerInstance.DebugError(fmt, args...)
}

func Warn(fmt string, args ...any) {
	LoggerInstance.Warn(fmt, args...)
}

func Error(fmt string, args ...any) {
	LoggerInstance.Error(fmt, args...)
}

func Fatal(fmt string, args ...any) {
	LoggerInstance.Fatal(fmt, args...)
}

func Panic(fmt string, args ...any) {
	LoggerInstance.Panic(fmt, args...)
}

func SetDebug(enable bool) {
	LoggerInstance.SetDebug(enable)
}

func SetStdOut(w io.Writer) {
	LoggerInstance.log.SetOutput(w)
}
