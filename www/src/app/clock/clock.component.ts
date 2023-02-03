import {Component, Input, OnDestroy, OnInit} from '@angular/core';
import {DateTime} from 'luxon';

@Component({
  selector: 'app-clock',
  templateUrl: './clock.component.html',
  styleUrls: ['./clock.component.css']
})
export class ClockComponent implements OnInit, OnDestroy {
  @Input() format: string;
  @Input() timeZone: string;
  private interval: any;
  public timeString: string = "";


  constructor() {
    this.format = "cccc dd LLL yyyy<br>HH:mm:ss";
    this.timeZone = 'UTC';
  }

  ngOnInit() {
    const that = this;
    setInterval(() => {
      that.timeString = DateTime.utc().setZone(this.timeZone).toFormat(this.format);
    }, 1000);
  }

  ngOnDestroy(): void {
    if (this.interval) {
      clearInterval(this.interval)
    }
  }

}
