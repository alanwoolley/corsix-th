import javax.swing.JComboBox;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextField;

import net.miginfocom.swing.MigLayout;


public class Emergency
{	
	static int emergencyMode = 2; 	// 0 = random;
	// 1 = semi-random
	// 2 = controlled emergencies;
	
	int startMonth = 3; //should become larger for each new add
	int endMonth = 6; //must automatically be >= startMonth
	int minPatients = 3;
	int maxPatients = 8; //must automatically be >= minPatients
	int illness = 16; //should be random on each new add, depending on existing diseases
	int percWin = 75;
	int bonus = 1000;
	String[] diseases = {"3rd Degree Sideburns","Alien DNA", "Baldness", "Bloaty Head",
			"Broken Bones", "Broken Heart", "Broken Wind", "Chronic Nosehair", "Corrugated Ankles",
			"Discrete Itching", "Elvis", "Fake Blood", "Gastric Ejections", "Golf Stones",
			"Gut Rot", "Hairyitus", "Heaped Piles", "Infectious Laughter", "Invisibility","Iron Lungs", 
			"Jellyitus", "Kidney Beans", "Multiple TV Personalities","Ruptured Nodules", "Serious Radiation",
			"Slack Tongue", "Sleeping Illness","Spare Ribs","Sweaty Palms",
			"The Squits", "Transparency", "Uncommon Cold","Unexpected Swelling" };
	
	JPanel emergencyPanel = new JPanel(new MigLayout("insets 1"));


	JComboBox illnessCombo = new JComboBox(diseases);
	JLabel minPatientsLabel = new JLabel("Patients min:");
	JLabel maxPatientsLabel = new JLabel("Patients max:");
	JLabel startMonthLabel = new JLabel("Start month:");
	JLabel endMonthLabel = new JLabel("End month:");
	JLabel percWinLabel = new JLabel("Win %:");
	JLabel bonusLabel = new JLabel("Bonus:");
	JTextField minPatientsTF = new JTextField(Integer.toString(minPatients), 2);
	JTextField maxPatientsTF = new JTextField(Integer.toString(maxPatients), 2);
	JTextField startMonthTF = new JTextField(Integer.toString(startMonth), 2);
	JTextField endMonthTF = new JTextField(Integer.toString(endMonth), 2);
	JTextField percWinTF = new JTextField(Integer.toString(percWin), 2);
	JTextField bonusTF = new JTextField(Integer.toString(bonus), 5);
	
}
