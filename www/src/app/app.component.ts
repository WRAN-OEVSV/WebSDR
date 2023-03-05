import { Component } from '@angular/core';
import {callsign, name} from './global-variables';
import {AuthenticationService} from "./authentication.service";

@Component({
  selector: 'app-root',
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.css']
})
export class AppComponent {
  title = name + "-" + callsign;

  constructor(public authenticationService: AuthenticationService) {
  }
}
