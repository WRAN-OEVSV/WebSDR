import { Component } from '@angular/core';
import {WebsocketService} from "../websocket.service";

@Component({
  selector: 'app-log-view',
  templateUrl: './log-view.component.html',
  styleUrls: ['./log-view.component.css']
})
export class LogViewComponent {

  events: Array<string> = [];

  constructor(public websocketService: WebsocketService) {
    this.websocketService.getEmitter().subscribe((log) => this.handleLog(log))
  }

  private handleLog(message: any) : void {
    if (message['log']) {
      const log = message.log.message;
      this.events.push(log);
      // limit length of the log in this view to 5 entries - otherwise we will get a very long log
      if (this.events.length > 5) {
        this.events.shift();
      }
    }
  }
}
