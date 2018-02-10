=======
Vagrant
=======
-------------------------------------------------------------
Návod pre použitie vagrantu pre vytvorenie VM na development
-------------------------------------------------------------
Pre použitie pripravenej VM je potrebné nainštalovať Vagrant,ansible a virtualbox.
Na host systéme s ubuntu 17.10 sa to robí  príkazmi:

    *$ sudo apt install ansible*
    
    *$ sudo apt install vagrant*
    
    *$ sudo apt install virtualbox*
    
prepnúť sa do adresáru kde sa nachádza Vagrantfile:

    *$ cd /path/to/3dsoftviz/vagrant*

použiť príkazy: 

    *$ vagrant plugin install vagrant-vbguest* // týmto sa zariadi funkčnosť opengl

Vo Vagrantfile sú definované parametre VM. Defaultne sú tam 4GB RAM,  treba znížiť pokiaľ nemáte aspoň 8GB RAM.

    *$ vagrant up*

Následne prebehne provisioning definovaný v jazyku ansible zo súboru playbook.yml
Sú tu definované balíky, ktoré treba nainštalovať pre build 3dsoftvizu. Pre inštaláciu balíkov sa používa apt package manager, čiže treba zachovať prvý príkaz v súbore playbook.yml kde sa apt nainštaluje. Po dokončení provisioningu je potrebné VM reštartovať.

Login do VM: vagrant

Heslo do VM: vagrant

Následne je možné v pripravenom prostredí buildnúť 3dsoftviz podla návodu k buildu
Je  potrebné vo vytvorenej VM dostať SSH klúč z host počítaču, alebo vygenerovať nový a pridať ho na github/gitlab!
