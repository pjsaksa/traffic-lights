package main

import (
	"fmt"
)

type Report interface {
	toJson() string
}

// ----- MessageReport

type MessageReport struct {
	message string
}

func NewMessageReport(message string) MessageReport {
	return MessageReport{
		message: message,
	}
}

func (report MessageReport) toJson() string {
	return fmt.Sprintf(
		`{ "report": "message", "data": "%s" }`,
		report.message)
}

// ----- ClaimSubReport

type ClaimSubReport struct {
	lane   string
	status string
}

func (report ClaimSubReport) toJson() string {
	return fmt.Sprintf(
		`{ "lane": "%s", "status": "%s" }`,
		report.lane,
		report.status)
}

// ----- ClaimsReport

type ClaimsReport struct {
	units []ClaimSubReport
}

func NewClaimsReport(crossing *Crossing) ClaimsReport {
	if len(crossing.claims) == 0 {
		return ClaimsReport{}
	}

	units := make([]ClaimSubReport, len(crossing.claims))

	index := 0
	for lane,cl := range crossing.claims {
		units[index].lane = lane.WebString()

		if cl.IsClaimed() {
			units[index].status = "green"
		} else {
			units[index].status = "red"
		}

		index++
	}

	return ClaimsReport{
		units: units,
	}
}

func (report ClaimsReport) toJson() string {
	data := `{ "report": "lane-status",
  "data": [
`

	index := 0
	for _,unit := range report.units {
		if index > 0 {
			data += ",\n"
		}
		data += "    " + unit.toJson()
		index++
	}

	data += "\n  ]\n}\n"

	return data
}
