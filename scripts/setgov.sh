#!/bin/bash

function setgov()
{
	for i in {0..40};
	do 
		cpufreq-set -c "$i" -g $1;
	done
}

setgov "userspace"
