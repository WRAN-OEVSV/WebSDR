import {Component} from '@angular/core';
import {MatDialog} from "@angular/material/dialog";
import {LoginFormComponent} from "./login-form/login-form.component";
import {AuthenticationService} from "../authentication.service";

@Component({
  selector: 'app-user-management',
  templateUrl: './user-management.component.html',
  styleUrls: ['./user-management.component.css']
})
export class UserManagementComponent {

  constructor(public dialog: MatDialog, public authenticationService: AuthenticationService) {}


  openDialog(): void {
    if (this.authenticationService.isAuthenticated()) {
      return;
    }
    let dialogRef = this.dialog.open(LoginFormComponent, {
      width: '500px', height: "230px",  data: { }
    });

    dialogRef.afterClosed().subscribe(result => {
      if (result.reason == 'login') {
          this.authenticationService.login(result.username, result.password);
      }
    });
  }

}
